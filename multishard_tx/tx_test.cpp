
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <cascade/service_client_api.hpp>

using namespace derecho::cascade;

// helper functions
void create_pool(ServiceClientAPI& capi,const std::string& path,uint32_t subgroup_index,const std::unordered_map<std::string,uint32_t>& object_locations = {});
ObjectWithStringKey create_object(std::string key,std::string value,persistent::version_t current_version = INVALID_VERSION,persistent::version_t prev_ver_key = INVALID_VERSION);
ObjectWithStringKey get_object(ServiceClientAPI& capi,std::string key);
persistent::version_t put_object(ServiceClientAPI& capi,ObjectWithStringKey& obj);
std::string id_to_string(transaction_id& txid);
std::string status_to_string(transaction_status_t status);
transaction_status_t poll_tx_completion(ServiceClientAPI& capi,transaction_id& txid);

int main(int argc, char** argv){
    auto& capi = ServiceClientAPI::get_service_client();

    // pool in sugroup 0 with 3 shards 
    create_pool(capi,"/sg0",0,{
                {"/sg0/0",0},
                {"/sg0/00",0},
                {"/sg0/1",1},
                {"/sg0/11",1},
                {"/sg0/2",2},
                {"/sg0/22",2},
            });

    std::vector<std::string> key_list = {
        "/sg0/0",
        "/sg0/00",
        "/sg0/1",
        "/sg0/11",
        "/sg0/2",
        "/sg0/22",
    };

    // first put all objects and get their versions
    std::unordered_map<std::string,persistent::version_t> version_map;
    for(auto& key : key_list) {
        ObjectWithStringKey obj = create_object(key,"new");

        // do it twice, so we get previous versions (by key) that are higher than -1 on a fresh persistent log (otherwise we can't test wrong version assignments)
        version_map[key] = put_object(capi,obj);
        version_map[key] = put_object(capi,obj);
    }

    // now we create four transactions: 
    //   TX1 should fail in shard 2, since a previous version is incorrect
    //   TX2 and TX3 are correct and identical, but only TX2 should suceed since it comes first and invalidate the versions for TX3
    //   TX4 should fail since one of the read-only versions is incorrect
    //   TX5 should succeed, since it is the same as TX4 but without the read-only object

    // lists of objects
    std::vector<ObjectWithStringKey> tx1_objects = {
        create_object("/sg0/0","tx1",INVALID_VERSION,version_map["/sg0/0"]),
        create_object("/sg0/1","tx1",INVALID_VERSION,version_map["/sg0/1"]),
        create_object("/sg0/2","tx1",INVALID_VERSION,version_map["/sg0/2"]-1), // using the wrong version
    };
    std::vector<ObjectWithStringKey> tx23_objects = {
        create_object("/sg0/0","tx23",INVALID_VERSION,version_map["/sg0/0"]),
        create_object("/sg0/1","tx23",INVALID_VERSION,version_map["/sg0/1"]),
        create_object("/sg0/2","tx23",INVALID_VERSION,version_map["/sg0/2"]), // correct now
    };
    std::vector<ObjectWithStringKey> tx45_objects = {
        create_object("/sg0/00","tx45",INVALID_VERSION,version_map["/sg0/00"]),
        create_object("/sg0/11","tx45",INVALID_VERSION,version_map["/sg0/11"]),
        create_object("/sg0/22","tx45",INVALID_VERSION,version_map["/sg0/22"]),
    };
    std::vector<ObjectWithStringKey> tx4_readonly_objects = {
        create_object("/sg0/1","",version_map["/sg0/1"]), // the version was changed by TX2
    };

    // send TXs
    auto tx1_res = capi.put_objects(tx1_objects);
    auto tx2_res = capi.put_objects(tx23_objects);
    auto tx3_res = capi.put_objects(tx23_objects);
    auto tx4_res = capi.put_objects(tx45_objects,tx4_readonly_objects);
    auto tx5_res = capi.put_objects(tx45_objects);

    // check results
    for (auto& reply_future : tx1_res.get()){
        auto reply = reply_future.second.get();
        transaction_id txid = reply.first;
        transaction_status_t status = poll_tx_completion(capi,txid);
        std::cout << "TX1 " << id_to_string(txid) << ": " << status_to_string(status) << std::endl; 
    }
    for (auto& reply_future : tx2_res.get()){
        auto reply = reply_future.second.get();
        transaction_id txid = reply.first;
        transaction_status_t status = poll_tx_completion(capi,txid);
        std::cout << "TX2 " << id_to_string(txid) << ": " << status_to_string(status) << std::endl; 
    }
    for (auto& reply_future : tx3_res.get()){
        auto reply = reply_future.second.get();
        transaction_id txid = reply.first;
        transaction_status_t status = poll_tx_completion(capi,txid);
        std::cout << "TX3 " << id_to_string(txid) << ": " << status_to_string(status) << std::endl; 
    }
    for (auto& reply_future : tx4_res.get()){
        auto reply = reply_future.second.get();
        transaction_id txid = reply.first;
        transaction_status_t status = poll_tx_completion(capi,txid);
        std::cout << "TX4 " << id_to_string(txid) << ": " << status_to_string(status) << std::endl; 
    }
    for (auto& reply_future : tx5_res.get()){
        auto reply = reply_future.second.get();
        transaction_id txid = reply.first;
        transaction_status_t status = poll_tx_completion(capi,txid);
        std::cout << "TX5 " << id_to_string(txid) << ": " << status_to_string(status) << std::endl; 
    }
}

// create object pool on the cascade volatile store
void create_pool(ServiceClientAPI& capi,const std::string& path,uint32_t subgroup_index,const std::unordered_map<std::string,uint32_t>& object_locations){
    // check if already exists
    auto opm = capi.find_object_pool(path);
    if (opm.is_valid() && !opm.is_null()) return;

    //auto res = capi.template create_object_pool<VolatileCascadeStoreWithStringKey>(path,subgroup_index,HASH,object_locations);
    auto res = capi.template create_object_pool<PersistentCascadeStoreWithStringKey>(path,subgroup_index,HASH,object_locations);
    for (auto& reply_future:res.get()) {
        auto reply = reply_future.second.get();
    }
}

ObjectWithStringKey create_object(std::string key,std::string value,persistent::version_t current_version,persistent::version_t prev_ver_key){
    ObjectWithStringKey obj;
    obj.key = key;
    obj.version = current_version;
    obj.previous_version_by_key = prev_ver_key;
    obj.blob = Blob(reinterpret_cast<const uint8_t*>(value.c_str()),value.size()+1);
    return obj;
}

std::string id_to_string(transaction_id& txid){
    auto subgroup_index = std::get<0>(txid);
    auto shard_index = std::get<1>(txid);
    auto version = std::get<2>(txid);

    std::stringstream ss;
    ss << "(" << subgroup_index << "," << shard_index << "," << version << ")";
    return ss.str();
}

std::string status_to_string(transaction_status_t status){
    switch(status){
        case PENDING:
            return "pending";
            break;
        case COMMIT:
            return "commit";
            break;
        case ABORT:
            return "abort";
            break;
        default:
            break;
    }
    return "unknown";
}

ObjectWithStringKey get_object(ServiceClientAPI& capi,std::string key){
    auto res = capi.get(key);
    for (auto& reply_future : res.get()){
        return reply_future.second.get();
    }

    ObjectWithStringKey obj;
    return obj;
}

persistent::version_t put_object(ServiceClientAPI& capi,ObjectWithStringKey& obj){
    persistent::version_t version = INVALID_VERSION;
    auto res = capi.put(obj);
    for (auto& reply_future : res.get()){
        version = std::get<0>(reply_future.second.get());
    }

    return version;
}

transaction_status_t poll_tx_completion(ServiceClientAPI& capi,transaction_id& txid){
    transaction_status_t status = transaction_status_t::PENDING;

    while(status == transaction_status_t::PENDING){
        auto res = capi.get_transaction_status<PersistentCascadeStoreWithStringKey>(txid);
        for (auto& reply_future : res.get()){
            status = reply_future.second.get();
        }
    }

    return status;
}


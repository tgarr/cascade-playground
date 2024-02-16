
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cascade/service_client_api.hpp>

using namespace derecho::cascade;

// helper functions
void create_pool(ServiceClientAPI& capi,const std::string& path,uint32_t subgroup_index,const std::unordered_map<std::string,uint32_t>& object_locations = {});
persistent::version_t write_objects(ServiceClientAPI& capi,std::vector<std::string>& key_list,std::string value,persistent::version_t previous_version = INVALID_VERSION);

int main(int argc, char** argv){
    auto& capi = ServiceClientAPI::get_service_client();

    create_pool(capi,"/test",0);
    //create_pool(capi,"/test",0,{{"/test/3",1}}); // should fail, since one object goes to a different shard

    std::vector<std::string> key_list = {
        "/test/1",
        "/test/2",
        "/test/3"
    };
    
    auto version = write_objects(capi,key_list,"new 1");
    version = write_objects(capi,key_list,"new 2");
    write_objects(capi,key_list,"update wrong",version-1); // should fail, since previous_version is incorrect and is not INVALID_VERSION
    write_objects(capi,key_list,"update correct",version);
}

// create object pool on the cascade volatile store
void create_pool(ServiceClientAPI& capi,const std::string& path,uint32_t subgroup_index,const std::unordered_map<std::string,uint32_t>& object_locations){
    // check if already exists
    auto opm = capi.find_object_pool(path);
    if (opm.is_valid() && !opm.is_null()) return;

    auto res = capi.template create_object_pool<VolatileCascadeStoreWithStringKey>(path,subgroup_index,HASH,object_locations);
    for (auto& reply_future:res.get()) {
        auto reply = reply_future.second.get();
    }
}

persistent::version_t write_objects(ServiceClientAPI& capi,std::vector<std::string>& key_list,std::string value,persistent::version_t previous_version){
    std::vector<ObjectWithStringKey> objects;

    // vector of objects
    for(std::string& key : key_list){
        ObjectWithStringKey obj;
        obj.key = key;
        obj.previous_version = previous_version;
        obj.blob = Blob(reinterpret_cast<const uint8_t*>(value.c_str()),value.size()+1);
        objects.push_back(obj);
    }
    
    // check version
    persistent::version_t version;
    auto res = capi.put_objects(objects);
    for (auto& reply_future : res.get()){
        auto reply = reply_future.second.get();
        version = std::get<0>(reply);
        std::cout << "Version: " << version;
        if(version == INVALID_VERSION)
            std::cout << " (error)";
        else
            std::cout << " (success)";
        std::cout << std::endl;
    }

    return version;
}


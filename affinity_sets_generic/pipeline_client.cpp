#include <iostream>
#include <string>
#include "common.hpp"

bool usage(int argc, char** argv){
    if(argc >= 4) return true;
    std::cout << argv[0] << " <object_size> <object_rate> <data_size>" << std::endl;
    return false;
}

void setup(ServiceClientAPI& capi,int object_size,int object_rate,int data_size){
    // affinity sets mapping function
    capi.set_affinity_set_logic(affinity_logic);

    // create necessary object pools for the client
    std::cout << "Creating object pools ... "; fflush(stdout);
    create_pool(capi,OBJ_ENTRY_PATH);
    create_pool(capi,OBJ_DATA_CATEGORY_PATH);
    std::cout << "done" << std::endl;
   
    std::cout << "List of object pools:" << std::endl;
    for (auto& op: capi.list_object_pools(true)) {
        std::cout << "\t" << op << std::endl;
    }

    // put data   

    ObjectWithStringKey obj;
    obj.key = "/entry/test";
    obj.previous_version = INVALID_VERSION;
    obj.previous_version_by_key = INVALID_VERSION;
    obj.blob = Blob(reinterpret_cast<const uint8_t*>(random_buffer(1000)),1000);
    auto res = capi.put(obj);
    for (auto& reply_future:res.get()) {
        auto reply = reply_future.second.get();
        std::cout << "node(" << reply_future.first << ") replied with version:" << std::get<0>(reply)
                  << ",ts_us:" << std::get<1>(reply)
                  << std::endl;
    }
}

void benchmark(ServiceClientAPI& capi,int object_size,int object_rate,int data_size){
}

int main(int argc, char** argv) {
    if(!usage(argc,argv)) return 0;

    // benchmark parameters   
    int object_size = std::stoi(argv[1]); 
    int object_rate = std::stoi(argv[2]); 
    int data_size = std::stoi(argv[3]); 

    // connect to service
    std::cout << "Connecting to Cascade ... "; fflush(stdout);
    ServiceClientAPI capi;
    std::cout << "done" << std::endl;

    // setup benchmark: object pools, data, and listener thread
    setup(capi,object_size,object_rate,data_size);
   
    // run benchmark 
    benchmark(capi,object_size,object_rate,data_size);

    return 0;
}


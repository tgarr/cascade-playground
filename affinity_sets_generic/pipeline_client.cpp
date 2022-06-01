#include <iostream>
#include <string>
#include <cascade/service_client_api.hpp>
#include "common.hpp"

using namespace derecho::cascade;

bool usage(int argc, char** argv){
    if(argc >= 4) return true;
    std::cout << argv[0] << " <object_size> <object_rate> <data_size>" << std::endl;
    return false;
}

void create_pool(ServiceClientAPI& capi,const std::string& path){
    auto res = capi.template create_object_pool<VolatileCascadeStoreWithStringKey>(path,0);
    for (auto& reply_future:res.get()) {
        auto reply = reply_future.second.get();
    }
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


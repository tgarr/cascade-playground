#ifndef _AFFINITY_BENCHMARK_COMMON_
#define _AFFINITY_BENCHMARK_COMMON_

#include <iostream>
#include <fstream>
#include <string>
#include <cascade/service_client_api.hpp>

using namespace derecho::cascade;

#define BENCHMARK_TIME 30

#ifndef NUM_CATEGORIES
#define NUM_CATEGORIES 10
#endif

#define OBJ_PATH_SEP "/"
#define OBJ_ENTRY_PATH OBJ_PATH_SEP "entry"
#define OBJ_CATEGORY_PATH OBJ_PATH_SEP "category"
#define OBJ_NEW_CATEGORY_KEY OBJ_PATH_SEP "new"
#define OBJ_OUTPUT_CATEGORY_KEY OBJ_PATH_SEP "output"
#define OBJ_DATA_CATEGORY_KEY OBJ_PATH_SEP "data"
#define OBJ_NEW_CATEGORY_PATH OBJ_CATEGORY_PATH OBJ_NEW_CATEGORY_KEY
#define OBJ_OUTPUT_CATEGORY_PATH OBJ_CATEGORY_PATH OBJ_OUTPUT_CATEGORY_KEY
#define OBJ_DATA_CATEGORY_PATH OBJ_CATEGORY_PATH OBJ_DATA_CATEGORY_KEY

#define ENTRY_AFFINITY_KEY "entry"

#define CLIENT_RETURN_UDP_PORT 43259
#define CLIENT_RETURN_TIMEOUT 10

// user defined affinity sets
const std::string affinity_logic(const std::string & key){
    if(key.find(OBJ_ENTRY_PATH) == 0){
        std::cout << "[AFFINITY_LOGIC] key: " << key << " | affinity key: " << ENTRY_AFFINITY_KEY << std::endl;
        return ENTRY_AFFINITY_KEY; // entry
    }

    std::string affinity_key = key;

    // category
    if(key.find(OBJ_CATEGORY_PATH) == 0 && key.size() > sizeof(OBJ_CATEGORY_PATH)){
        std::string key2 = key.substr(sizeof(OBJ_CATEGORY_PATH)-1);

        // new
        if(key2.find(OBJ_NEW_CATEGORY_KEY) == 0 && key2.size() > sizeof(OBJ_NEW_CATEGORY_KEY)){
            std::string key3 = key2.substr(sizeof(OBJ_NEW_CATEGORY_KEY));
            std::string::size_type pos = key3.find(OBJ_PATH_SEP);
            if(pos != std::string::npos) affinity_key = key3.substr(0,pos);
        }
        // output
        else if(key2.find(OBJ_OUTPUT_CATEGORY_KEY) == 0 && key2.size() > sizeof(OBJ_OUTPUT_CATEGORY_KEY)){
            std::string key3 = key2.substr(sizeof(OBJ_OUTPUT_CATEGORY_KEY));
            std::string::size_type pos = key3.find(OBJ_PATH_SEP);
            if(pos != std::string::npos) affinity_key = key3.substr(0,pos);
        }
        // data
        else if(key2.find(OBJ_DATA_CATEGORY_KEY) == 0 && key2.size() > sizeof(OBJ_DATA_CATEGORY_KEY)){
            affinity_key = key2.substr(sizeof(OBJ_DATA_CATEGORY_KEY));
        }
    
        if(affinity_key.size() == 0) affinity_key = key;
    }

    std::cout << "[AFFINITY_LOGIC] key: " << key << " | affinity key: " << affinity_key << std::endl;

    return affinity_key;
}

/*const std::string affinity_logic(const std::string & key){
    return key;
}*/

char * random_buffer(int size){
    std::ifstream rndfile;
    char *buffer = new char[size];

    rndfile.open("/dev/random");
    rndfile.read(buffer,size);
    rndfile.close();

    return buffer;
}

void create_pool(ServiceClientAPI& capi,const std::string& path){
    // check if already exists
    auto opm = capi.find_object_pool(path);
    if (opm.is_valid() && !opm.is_null()) return;

    auto res = capi.template create_object_pool<VolatileCascadeStoreWithStringKey>(path,0);
    for (auto& reply_future:res.get()) {
        auto reply = reply_future.second.get();
    }
}

#endif


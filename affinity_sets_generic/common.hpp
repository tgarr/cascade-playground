#ifndef _AFFINITY_BENCHMARK_COMMON_
#define _AFFINITY_BENCHMARK_COMMON_

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <chrono>
#include <random>
#include <cascade/service_client_api.hpp>

using namespace derecho::cascade;

#ifndef BENCHMARK_TIME
#define BENCHMARK_TIME 5
#endif

#ifndef NUM_CATEGORIES
#define NUM_CATEGORIES 10
#endif

#define OBJ_PATH_SEP "/"
#define OBJ_ENTRY_PATH OBJ_PATH_SEP "entry"
#define OBJ_CATEGORY_PATH OBJ_PATH_SEP "category"
#define OBJ_NEW_CATEGORY_KEY OBJ_PATH_SEP "new"
#define OBJ_OUTPUT_CATEGORY_KEY OBJ_PATH_SEP "output"
#define OBJ_DATA_CATEGORY_KEY OBJ_PATH_SEP "data"

#define OBJ_CONFIG_PATH OBJ_PATH_SEP "config"
#define OBJ_CONFIG_ENTRY_PART_SIZE OBJ_CONFIG_PATH OBJ_PATH_SEP "entry_part_size"
#define OBJ_CONFIG_NUM_ENTRY_PARTS OBJ_CONFIG_PATH OBJ_PATH_SEP "num_entry_parts"
#define OBJ_CONFIG_DATA_PART_SIZE OBJ_CONFIG_PATH OBJ_PATH_SEP "data_part_size"
#define OBJ_CONFIG_NUM_DATA_PARTS OBJ_CONFIG_PATH OBJ_PATH_SEP "num_data_parts"
#define OBJ_CONFIG_CLIENT_DATA OBJ_CONFIG_PATH OBJ_PATH_SEP "client_"

#define OBJ_NEW_CATEGORY_PATH OBJ_CATEGORY_PATH OBJ_NEW_CATEGORY_KEY
#define OBJ_OUTPUT_CATEGORY_PATH OBJ_CATEGORY_PATH OBJ_OUTPUT_CATEGORY_KEY
#define OBJ_DATA_CATEGORY_PATH OBJ_CATEGORY_PATH OBJ_DATA_CATEGORY_KEY

#define ENTRY_AFFINITY_KEY "entry"

#define CLIENT_RETURN_UDP_PORT 43259
#define CLIENT_RETURN_TIMEOUT BENCHMARK_TIME
#define RETURN_MESSAGE_SIZE sizeof(int)*3

#define CLIENT_SEED 3
#define RANDOM_BUFFER_CHUNK 100

static std::mt19937 cascade_client_rng(CLIENT_SEED);

#if AFFINITY_LOGIC == 0
const std::string affinity_logic(const std::string & key){
    return key;
}
#elif AFFINITY_LOGIC == 1
// user defined affinity sets
const std::string affinity_logic(const std::string & key){
    // entry
    if(key.find(OBJ_ENTRY_PATH) == 0) return ENTRY_AFFINITY_KEY;

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
            std::string key3 = key2.substr(sizeof(OBJ_DATA_CATEGORY_KEY));
            std::string::size_type pos = key3.find(OBJ_PATH_SEP);
            if(pos != std::string::npos) affinity_key = key3.substr(0,pos);
        }
    
        if(affinity_key.size() == 0) affinity_key = key;
    }

    return affinity_key;
}
#elif AFFINITY_LOGIC == 2
const std::string affinity_logic(const std::string & key){
    // entry
    if(key.find(OBJ_ENTRY_PATH) == 0) return key;

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
            std::string key3 = key2.substr(sizeof(OBJ_DATA_CATEGORY_KEY));
            std::string::size_type pos = key3.find(OBJ_PATH_SEP);
            if(pos != std::string::npos) affinity_key = key3.substr(0,pos);
        }
    
        if(affinity_key.size() == 0) affinity_key = key;
    }

    return affinity_key;
}
#endif

void set_client_seed(int seed){
    cascade_client_rng.seed(seed);
}

char * random_buffer(int size){
    char *buffer = new char[size];

    int already_set = 0;
    while((already_set+RANDOM_BUFFER_CHUNK) <= size){
        memset(buffer+already_set,cascade_client_rng() % 256,RANDOM_BUFFER_CHUNK);
        already_set += RANDOM_BUFFER_CHUNK;
    }

    int remaining = size - already_set;
    if(remaining > 0) memset(buffer+already_set,cascade_client_rng() % 256,remaining);

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

std::chrono::high_resolution_clock::time_point put_object(ServiceClientAPI& capi, ObjectWithStringKey& obj){
    auto res = capi.put(obj);
    auto now = std::chrono::high_resolution_clock::now();
    for (auto& reply_future:res.get()){
        auto reply = reply_future.second.get();
    }
    return now;
}

std::chrono::high_resolution_clock::time_point put_random_object(ServiceClientAPI& capi,const std::string& key,int size){
    ObjectWithStringKey obj;
    obj.key = key;
    obj.previous_version = INVALID_VERSION;
    obj.previous_version_by_key = INVALID_VERSION;

    char* buffer = random_buffer(size);
    obj.blob = Blob(reinterpret_cast<const uint8_t*>(buffer),size);
    delete buffer;

    return put_object(capi,obj);
}

std::size_t hash_blob(const uint8_t* bytes, std::size_t size){
    return std::_Hash_bytes(bytes,size,0);
}

void put_config_object(ServiceClientAPI& capi,std::string key,int value){
    ObjectWithStringKey obj;
    obj.key = key;
    obj.previous_version = INVALID_VERSION;
    obj.previous_version_by_key = INVALID_VERSION;
    obj.blob = Blob(reinterpret_cast<const uint8_t*>(&value),sizeof(value));
    put_object(capi,obj);
}

void put_config_object(ServiceClientAPI& capi,std::string key,char* value){
    ObjectWithStringKey obj;
    obj.key = key;
    obj.previous_version = INVALID_VERSION;
    obj.previous_version_by_key = INVALID_VERSION;
    obj.blob = Blob(reinterpret_cast<const uint8_t*>(value),std::strlen(value)+1);
    put_object(capi,obj);
}

int get_config_int(ServiceClientAPI& capi,std::string key){
    auto res = capi.get(key);
    for (auto& reply_future:res.get()){
        auto obj = reply_future.second.get();
        return *reinterpret_cast<const int*>(obj.blob.bytes);
    }

    return -1;
}

char* get_config_str(ServiceClientAPI& capi,std::string key){
    char* str;
    auto res = capi.get(key);
    for (auto& reply_future:res.get()){
        auto obj = reply_future.second.get();
        str = new char[obj.blob.size];
        memcpy(str,reinterpret_cast<const char*>(obj.blob.bytes),obj.blob.size);
    }

    return str;
}

#endif


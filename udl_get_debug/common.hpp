#ifndef _UDL_GET_DEBUG_COMMON_
#define _UDL_GET_DEBUG_COMMON_

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <chrono>
#include <random>
#include <unordered_map>
#include <sched.h>
#include <cascade/service_client_api.hpp>

using namespace derecho::cascade;

#define TLT_UDLGET(x) (8000 + (x))

#define CLIENT_OBJECT_SIZE 100 // size of client request
#define SUBGROUP_INDEX 0 // all in the same subgroup
#define SHARD_INDEX 0 // shard that will store all udl objects

// cascade paths
#define CLIENT_REQUEST_PATH "/request"
#define UDL_REQUEST_PATH "/udl" 

// random object generation
#define CLIENT_SEED 49357
#define RANDOM_BUFFER_CHUNK 100
static std::mt19937 cascade_client_rng(CLIENT_SEED);

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

// create object pool on the cascade volatile store
void create_pool(ServiceClientAPI& capi,const std::string& path,const std::unordered_map<std::string,uint32_t>& object_locations = {}){
    // check if already exists
    auto opm = capi.find_object_pool(path);
    if (opm.is_valid() && !opm.is_null()) return;

    auto res = capi.template create_object_pool<VolatileCascadeStoreWithStringKey>(path,SUBGROUP_INDEX,HASH,object_locations);
    for (auto& reply_future:res.get()) {
        auto reply = reply_future.second.get();
    }
}

// put a given object, wait, and return the timestamp of the put
std::chrono::high_resolution_clock::time_point put_object(ServiceClientAPI& capi, ObjectWithStringKey& obj){
    auto now = std::chrono::high_resolution_clock::now();
    auto res = capi.put(obj);
    for (auto& reply_future:res.get()){
        auto reply = reply_future.second.get();
    }
    return now;
}

// put a random object, wait, and return the timestamp of the put
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

// object paths

std::string client_request_path(const std::string &key){
    return CLIENT_REQUEST_PATH "/" + key;
}

std::string udl_request_path(const std::string &key){
    return UDL_REQUEST_PATH "/" + key;
}

// other stuff

void cpu_affinity(int core_id){
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id,&cpuset);
    sched_setaffinity(0,sizeof(cpu_set_t),&cpuset);
}

#endif


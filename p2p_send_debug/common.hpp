#ifndef _P2P_SEND_DEBUG_COMMON_
#define _P2P_SEND_DEBUG_COMMON_

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

// duration in number of objects sent by client
#ifndef EXPERIMENT_DURATION
#define EXPERIMENT_DURATION 1500
#endif

// throughput
#ifndef OBJECT_RATE
#define OBJECT_RATE 50
#endif

// size of objects sent by client
#ifndef CLIENT_OBJECT_SIZE
#define CLIENT_OBJECT_SIZE 1000
#endif

// number of objects to be get by the UDL
#ifndef UDL_OBJECTS_NUM
#define UDL_OBJECTS_NUM 100
#endif

// size of objects to be get by the UDL
#ifndef UDL_OBJECTS_SIZE
//#define UDL_OBJECTS_SIZE 100000 // 100KB
#define UDL_OBJECTS_SIZE 1000000 // 1MB
#endif

#define SUBGROUP_INDEX 0
#define STORAGE_SHARD_INDEX 0
#define REMOTE_SHARD_INDEX 1

#define CLIENT_SEED 49357
#define RANDOM_BUFFER_CHUNK 100

// cascade paths
#define LOCAL_OBJECT_PATH "/local/"
#define REMOTE_OBJECT_PATH "/remote/"
#define UDL_OBJECT_PATH "/udl/"

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

void create_pool(ServiceClientAPI& capi,const std::string& path,const std::unordered_map<std::string,uint32_t>& object_locations = {}){
    // check if already exists
    auto opm = capi.find_object_pool(path);
    if (opm.is_valid() && !opm.is_null()) return;

    auto res = capi.template create_object_pool<VolatileCascadeStoreWithStringKey>(path,SUBGROUP_INDEX,HASH,object_locations);
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

void cpu_affinity(int core_id){
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id,&cpuset);
    sched_setaffinity(0,sizeof(cpu_set_t),&cpuset);
}

std::string local_object_path(const std::string &key){
    return LOCAL_OBJECT_PATH + key;
}

std::string remote_object_path(const std::string &key){
    return REMOTE_OBJECT_PATH + key;
}

std::string udl_object_path(const std::string &key){
    return UDL_OBJECT_PATH + key;
}

#endif


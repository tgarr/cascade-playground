#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <chrono>
#include <random>
#include <unordered_map>
#include <sched.h>
#include <cascade/service_client_api.hpp>
#include <pthread.h>

using namespace derecho::cascade;

#define TLT_UDLGET(x) (8000 + (x))
#define CLIENT_MIN_WAIT 5

#define SUBGROUP_INDEX 0 // all in the same subgroup
#define LOCAL_SHARD_INDEX 0 // shard that will store all udl objects
#define REMOTE_SHARD_INDEX 1 // shard that will store all udl objects

// cascade paths
#define CLIENT_REQUEST_POOL "/request"
#define CLIENT_LOCAL_REQUEST_PATH CLIENT_REQUEST_POOL "/local"
#define CLIENT_REMOTE_REQUEST_PATH CLIENT_REQUEST_POOL "/remote"
#define UDL_REQUEST_POOL "/udl"
#define UDL_DATA_REQUEST_PATH UDL_REQUEST_POOL "/data"

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
void put_object(ServiceClientAPI& capi, ObjectWithStringKey& obj){
    auto res = capi.put(obj);
    for (auto& reply_future:res.get()){
        auto reply = reply_future.second.get();
    }
}

// put a random object, wait, and return the timestamp of the put
void put_random_object(ServiceClientAPI& capi,const std::string& key,int size){
    ObjectWithStringKey obj;
    obj.key = key;
    obj.previous_version = INVALID_VERSION;
    obj.previous_version_by_key = INVALID_VERSION;

    char* buffer = random_buffer(size);
    obj.blob = Blob(reinterpret_cast<const uint8_t*>(buffer),size);
    delete buffer;

    put_object(capi,obj);
}

void request_udl(ServiceClientAPI& capi,const std::string& key,int total){
    ObjectWithStringKey obj;
    obj.key = key;
    obj.previous_version = INVALID_VERSION;
    obj.previous_version_by_key = INVALID_VERSION;

    obj.blob = Blob(reinterpret_cast<const uint8_t*>(&total),sizeof(total));

    return put_object(capi,obj);
}

void local_request(ServiceClientAPI& capi,int total){
    return request_udl(capi,CLIENT_LOCAL_REQUEST_PATH,total);
}

void remote_request(ServiceClientAPI& capi,int total){
    return request_udl(capi,CLIENT_REMOTE_REQUEST_PATH,total);
}

// other stuff

void cpu_affinity(int core_id){
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id,&cpuset);

    pthread_t current_thread = pthread_self();
    pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}


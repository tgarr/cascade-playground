#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "common.hpp"

bool usage(int argc, char** argv){
    bool ok = true;
    if(argc < 2) ok = false;
    else {
        std::string mode(argv[1]);
        if((mode != "local") && (mode != "remote")) ok = false;
    }
    if(!ok) std::cout << argv[0] << " <local|remote>" << std::endl;
    return ok;
}

void experiment(ServiceClientAPI& capi,std::string &mode,int duration,int object_rate,int object_size){
    std::cout << "Starting experiment in " << mode << " mode ..." << std::endl;

    auto period = std::chrono::nanoseconds(1000000000) / object_rate;
    int object_id = 0;
    while(object_id < duration){
        auto start = std::chrono::high_resolution_clock::now();

        // put new object
        std::string key;
        if(mode == "local") key = local_object_path(std::to_string(object_id));
        else key = remote_object_path(std::to_string(object_id));
        put_random_object(capi,key,object_size);

        if(object_id % 100 == 0)
            std::cout << "  object " << object_id << "/" << duration << std::endl;

        object_id++;

        // sleep
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        if(elapsed < period) std::this_thread::sleep_for(period - elapsed);
    }

    std::cout << "Experiment finished!" << std::endl;
}

int main(int argc, char** argv) {
    if(!usage(argc,argv)) return 0;
    cpu_affinity(1);

    // connect to service
    std::cout << "Connecting to Cascade ... "; fflush(stdout);
    ServiceClientAPI capi;
    std::cout << "done" << std::endl;

    // parameters
    std::string mode(argv[1]);
    int duration = EXPERIMENT_DURATION;
    int object_rate = OBJECT_RATE;
    int object_size = CLIENT_OBJECT_SIZE;

    experiment(capi,mode,duration,object_rate,object_size);

    return 0;
}


#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "common.hpp"

bool usage(int argc, char** argv){
    if(argc < 4){
        std::cout << "usage: " << argv[0] << "<request_rate> <total_requests> <udl_request_size>" << std::endl;
        return false;
    }
    return true;
}

void experiment(ServiceClientAPI& capi,int duration,int object_rate){
    std::cout << "Starting experiment ..." << std::endl;

    auto period = std::chrono::nanoseconds(1000000000) / object_rate;
    int object_id = 0;
    while(object_id < duration){
        auto start = std::chrono::high_resolution_clock::now();

        // put new object
        std::string key = client_request_path(std::to_string(object_id));
        put_random_object(capi,key,CLIENT_OBJECT_SIZE);

        if(object_id % 100 == 0)
            std::cout << "  object " << object_id << "/" << duration << std::endl;

        object_id++;

        // sleep
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        if(elapsed < period) std::this_thread::sleep_for(period - elapsed);
    }

    std::cout << "Experiment finished!" << std::endl;
}

void setup(ServiceClientAPI& capi,int duration,int udl_objects_size){
    // object pools
    std::cout << "Creating object pools ... "; fflush(stdout);
    create_pool(capi,CLIENT_REQUEST_PATH);
    create_pool(capi,UDL_REQUEST_PATH);
    std::cout << "done" << std::endl;

    // put udl object
    std::cout << "Putting UDL objects ... "; fflush(stdout);
    put_random_object(capi,udl_request_path("data"),udl_objects_size);
    std::cout << "done" << std::endl;
}

int main(int argc, char** argv) {
    if(!usage(argc,argv)) return 0;
    cpu_affinity(1);

    // connect to service
    std::cout << "Connecting to Cascade ... "; fflush(stdout);
    ServiceClientAPI capi;
    std::cout << "done" << std::endl;

    // parameters
    int request_rate = std::stoi(argv[1]);
    int total_requests = std::stoi(argv[2]);
    int udl_request_size = std::stoi(argv[3]);

    setup(capi,total_requests,udl_request_size);
    experiment(capi,total_requests,request_rate);

    return 0;
}


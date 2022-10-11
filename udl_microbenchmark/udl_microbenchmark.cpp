#include <iostream>
#include <string>
#include <algorithm>
#include "common.hpp"

bool usage(int argc, char** argv){
    bool ok = true;
    if(argc < 4) ok = false;
    else{
        std::string mode(argv[1]);
        if((mode != "local") && (mode != "remote")) ok = false;
    }
    
    if(!ok){
        std::cout << "usage: " << argv[0] << " <local|remote> <total_requests> <udl_request_size>" << std::endl;
    }

    return ok;
}

void experiment(ServiceClientAPI& capi,std::string &mode,int duration){
    std::cout << "Sending request (" << mode << ") ..." << std::endl;

    if(mode == "local"){
        local_request(capi,duration);
    }
    else if(mode == "remote"){
        remote_request(capi,duration);
    }

    std::cout << "Done!" << std::endl;
}

void setup(ServiceClientAPI& capi,int udl_objects_size){
    // object pools
    std::cout << "Creating object pools ... "; fflush(stdout);
    create_pool(capi,CLIENT_REQUEST_POOL,{{CLIENT_LOCAL_REQUEST_PATH,LOCAL_SHARD_INDEX},{CLIENT_REMOTE_REQUEST_PATH,REMOTE_SHARD_INDEX}});
    create_pool(capi,UDL_REQUEST_POOL,{{UDL_DATA_REQUEST_PATH,LOCAL_SHARD_INDEX}});
    std::cout << "done" << std::endl;

    // put udl object
    std::cout << "Putting UDL object ... "; fflush(stdout);
    put_random_object(capi,UDL_DATA_REQUEST_PATH,udl_objects_size);
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
    std::string mode(argv[1]);
    int total_requests = std::stoi(argv[2]);
    int udl_request_size = std::stoi(argv[3]);

    setup(capi,udl_request_size);
    experiment(capi,mode,total_requests);

    return 0;
}


#include <iostream>
#include <string>
#include <unordered_map>
#include "common.hpp"

bool usage(int argc, char** argv){
    if(argc < 2) {
        std::cout << argv[0] << " <udl_object_size>" << std::endl;
        return false;
    }
    return true;
}

void setup(ServiceClientAPI& capi,int duration,int udl_objects_num,int udl_objects_size){
    // map client objects to the correct shard (where the UDL will run)
    std::unordered_map<std::string,uint32_t> local,remote;
    for(int i=0;i<duration;i++){
        local[local_object_path(std::to_string(i))] = STORAGE_SHARD_INDEX;
        remote[remote_object_path(std::to_string(i))] = REMOTE_SHARD_INDEX;
    }

    // map udl objects to the correct shard (from where the UDL always gets them)
    std::unordered_map<std::string,uint32_t> udl;
    for(int i=0;i<udl_objects_num;i++){
        udl[udl_object_path(std::to_string(i))] = STORAGE_SHARD_INDEX;
    }
        
    // object pools
    std::cout << "Creating object pools ... "; fflush(stdout);
    create_pool(capi,LOCAL_OBJECT_PATH,local);
    create_pool(capi,REMOTE_OBJECT_PATH,remote);
    create_pool(capi,UDL_OBJECT_PATH,udl);
    std::cout << "done" << std::endl;

    // putting udl objects
    std::cout << "Putting UDL objects ... "; fflush(stdout);
    for(int i=0;i<udl_objects_num;i++){
        put_random_object(capi,udl_object_path(std::to_string(i)),udl_objects_size);
    }
    std::cout << "done" << std::endl;
}

int main(int argc, char** argv) {
    if(!usage(argc,argv)) return 0;

    // connect to service
    std::cout << "Connecting to Cascade ... "; fflush(stdout);
    ServiceClientAPI capi;
    std::cout << "done" << std::endl;

    // parameters
    int duration = EXPERIMENT_DURATION;
    int udl_objects_num = UDL_OBJECTS_NUM;
    //int udl_objects_size = UDL_OBJECTS_SIZE;
    int udl_objects_size = std::stoi(argv[1]);

    setup(capi,duration,udl_objects_num,udl_objects_size);

    return 0;
}


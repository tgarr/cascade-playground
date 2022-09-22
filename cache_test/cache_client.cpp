#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "common.hpp"

void command(ServiceClientAPI& capi,int preput){
    std::cout << "Pre-caching " << preput << " objects ..." << std::endl;

    for(int i=0;i<preput;i++){
        std::string key = CLIENT_CACHE_REQUEST + "/" + std::to_string(i);
        std::cout << "  caching " << key << std::endl;
        put_random_object(capi,key,CLIENT_OBJECT_SIZE);
    }

    std::string cmd("");
    while(cmd != "quit"){
        std::cout << "command: ";
        std::getline(std::cin,cmd);

        int sep = cmd.find(" ");
        if(sep == std::string::npos) continue;

        std::string op = cmd.substr(0,sep);
        std::string arg = cmd.substr(sep+1,cmd.size()-1);

        if(op == "cache"){
            std::string key = CLIENT_CACHE_REQUEST + ("/" + arg);
            put_random_object(capi,key,CLIENT_OBJECT_SIZE);
        }
        else if(op == "check"){
            std::string key = CLIENT_CACHE_REQUEST + ("/" + arg);
            ObjectWithStringKey obj;
            obj.key = std::sting(CLIENT_CACHE_CHECK) + "/check";
            obj.previous_version = INVALID_VERSION;
            obj.previous_version_by_key = INVALID_VERSION;
            obj.blob = Blob(reinterpret_cast<const uint8_t*>(key.c_str()),key.size()+1);

            put_object(capi,obj);
        }
    }
}

int main(int argc, char** argv) {
    cpu_affinity(1);

    int preput = std::stoi(argv[1]);

    // connect to service
    ServiceClientAPI capi;

    command(capi,preput);

    return 0;
}


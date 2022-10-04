#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "common.hpp"

void command(ServiceClientAPI& capi,int preput){
    std::string cmd("");
    while(cmd != "quit"){
        std::cout << "command: ";
        std::getline(std::cin,cmd);

        int sep = cmd.find(" ");
        if(sep == std::string::npos) continue;

        std::string op = cmd.substr(0,sep);
        std::string arg = cmd.substr(sep+1,cmd.size()-1);

        if(op == "store"){
            int sep2 = arg.find(" ");
            std::string key = CLIENT_STORE_REQUEST + ("/" + arg.substr(0,sep2););
            std::string data = arg.substr(sep2+1,arg.size()-1);
            
            ObjectWithStringKey obj;
            obj.key = key;
            obj.previous_version = INVALID_VERSION;
            obj.previous_version_by_key = INVALID_VERSION;
            obj.blob = Blob(reinterpret_cast<const uint8_t*>(data.c_str()),data.size()+1);

            put_object(capi,obj);
        }
        else if(op == "check"){
            std::string key = CLIENT_STORE_REQUEST + ("/" + arg);
            ObjectWithStringKey obj;
            obj.key = std::string(CLIENT_STORE_CHECK) + "/check";
            obj.previous_version = INVALID_VERSION;
            obj.previous_version_by_key = INVALID_VERSION;
            obj.blob = Blob(reinterpret_cast<const uint8_t*>(key.c_str()),key.size()+1);

            put_object(capi,obj);
        }
    }
}

int main(int argc, char** argv) {
    cpu_affinity(1);

    // connect to service
    ServiceClientAPI capi;
    create_pool(capi,CLIENT_STORE_REQUEST);
    create_pool(capi,CLIENT_STORE_CHECK);

    command(capi);

    return 0;
}


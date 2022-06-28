#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "common.hpp"

namespace derecho{
namespace cascade{

#define UDL_UUID "b45678de-7a88-4afa-b571-6c87a2f30b01"
#define UDL_DESC "Notify the client that the pipeline is finished."

void notify_client(const char* ip_address,int* values,int client_id){
    struct sockaddr_in servaddr;

    // create socket
    int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

    // server addr
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(CLIENT_RETURN_UDP_PORT + client_id);
    servaddr.sin_addr.s_addr = inet_addr(ip_address);

    const char* buffer = (const char *)values;
    sendto(sockfd,buffer,sizeof(values),MSG_CONFIRM,(const struct sockaddr *) &servaddr,sizeof(servaddr));

    close(sockfd);
}

class ReturnObserver: public OffCriticalDataPathObserver {
    virtual void operator () (const node_id_t sender,
                              const std::string& key_string,
                              const uint32_t prefix_length,
                              persistent::version_t version,
                              const mutils::ByteRepresentable* const value_ptr,
                              const std::unordered_map<std::string,bool>& outputs,
                              ICascadeContext* ctxt,
                              uint32_t worker_id) override {

        // extract category, object_id and client_id
        int values[2];
        std::string key_values = key_string.substr(prefix_length);
        std::string::size_type pos = key_values.find(OBJ_PATH_SEP);
        std::string obj_id = key_values.substr(pos+1);
        values[1] = std::stoi(key_values.substr(0,pos));

        pos = obj_id.find("_");
        std::string client_id = obj_id.substr(0,pos);
        values[0] = std::stoi(obj_id.substr(pos+1));
        
        // std::cout << "[RETURN] received: " << key_string << " | category: " << values[1] << " | id: " << values[0] << " | client_id: " << client_id << std::endl;

        // send UDP packet
        auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);
        std::string config_key = OBJ_CONFIG_CLIENT_DATA + client_id;
        char* client_ip = get_config_str(typed_ctxt->get_service_client_ref(),config_key);

        // std::cout << "[RETURN] notifying client: " << client_ip << std::endl;
        notify_client(client_ip,values,std::stoi(client_id));
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<ReturnObserver>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }
};

std::shared_ptr<OffCriticalDataPathObserver> ReturnObserver::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    // set affinity sets logic
    auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);
    typed_ctxt->get_service_client_ref().set_affinity_set_logic(affinity_logic);

    //initialize observer
    ReturnObserver::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json&) {
    return ReturnObserver::get();
}

void release(ICascadeContext* ctxt) {
    // nothing to release
    return;
}

std::string get_uuid() {
    return UDL_UUID;
}

std::string get_description() {
    return UDL_DESC;
}

} // namespace cascade
} // namespace derecho



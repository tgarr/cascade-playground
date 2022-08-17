#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include "common.hpp"

namespace derecho{
namespace cascade{

#define UDL_UUID "8eb917ad-5c51-44de-be0c-2d2803898fd1"
#define UDL_DESC "Get objects and measure the p2p_send latency"

class DebugObserver: public OffCriticalDataPathObserver {
    virtual void operator () (const node_id_t sender,
                              const std::string& key_string,
                              const uint32_t prefix_length,
                              persistent::version_t version,
                              const mutils::ByteRepresentable* const value_ptr,
                              const std::unordered_map<std::string,bool>& outputs,
                              ICascadeContext* ctxt,
                              uint32_t worker_id) override {

        std::string obj_id = key_string.substr(prefix_length);
        auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);
        std::vector<derecho::rpc::QueryResults<const derecho::cascade::ObjectWithStringKey>> queries;
        std::vector<long long int> p2p_send_latencies;

        // send gets
        for(int i=0;i<UDL_OBJECTS_NUM;i++){
            std::string key = udl_object_path(std::to_string(i));
            long long int latency;
            
            res.push_back(typed_ctxt->get_service_client_ref().get(key,CURRENT_VERSION,false,&latency));
            p2p_send_latencies.push_back(latency);
        }

        // wait futures
        std::vector<long long int> wait_latencies;
        for(int i=0;i<UDL_OBJECTS_NUM;i++){
            auto start = std::chrono::high_resolution_clock::now();
            for (auto& reply_future:res[i].get()){
                auto obj = reply_future.second.get();
            }
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
            wait_latencies.push_back(latency.count()); 
        }

        // print results
        // TODO
        std::cerr << "P2P_SEND " << obj_id << std::endl; 
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<DebugObserver>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }
};

std::shared_ptr<OffCriticalDataPathObserver> DebugObserver::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    //initialize observer
    DebugObserver::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json&) {
    return DebugObserver::get();
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



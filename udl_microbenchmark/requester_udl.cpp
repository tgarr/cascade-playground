#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "common.hpp"
#include <cascade/utils.hpp>

namespace derecho{
namespace cascade{

#define UDL_UUID "8eb917ad-5c51-44de-be0c-2d2803898fd1"
#define UDL_DESC "Request many objects"

class RequesterObserver: public OffCriticalDataPathObserver {
    virtual void operator () (const node_id_t sender,
                              const std::string& key_string,
                              const uint32_t prefix_length,
                              persistent::version_t version,
                              const mutils::ByteRepresentable* const value_ptr,
                              const std::unordered_map<std::string,bool>& outputs,
                              ICascadeContext* ctxt,
                              uint32_t worker_id) override {

        auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);
        auto &capi = typed_ctxt->get_service_client_ref();
        int my_id = capi.get_my_id();
        
        // unpack request
        const auto* const request = dynamic_cast<const ObjectWithStringKey* const>(value_ptr);
        int *parameters = reinterpret_cast<int*>(request->blob.bytes);
        int rate = parameters[0];
        int total = parameters[1];
        
        std::cout << "[UDL][" << my_id << "] starting to get " << total << "objects, " << rate << "per second" << std::endl;

        // send get requests in the given rate
        std::vector<derecho::rpc::QueryResults<const derecho::cascade::ObjectWithStringKey>> requests;
        auto period = std::chrono::nanoseconds(1000000000) / rate;
        for(int i=0;i<total;i++){
            // start time
            auto start = std::chrono::high_resolution_clock::now();

            // send request
            global_timestamp_logger.log(TLT_UDLGET(1),my_id,i,get_walltime());
            auto request = capi.get(UDL_DATA_REQUEST_PATH,CURRENT_VERSION,false);
            global_timestamp_logger.log(TLT_UDLGET(2),my_id,i,get_walltime());
            requests.push_back(request);

            // sleep
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            if(elapsed < period) std::this_thread::sleep_for(period - elapsed);
        }

        // wait for replies
        for(int i=0;i<total;i++){
            for (auto& reply_future:requests[i].get()){
                auto obj = reply_future.second.get();
            }
            global_timestamp_logger.log(TLT_UDLGET(3),my_id,i,get_walltime());
        }

        std::cout << "[UDL][" << my_id << "] finished" << std::endl;
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<RequesterObserver>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }
};

std::shared_ptr<OffCriticalDataPathObserver> RequesterObserver::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    //initialize observer
    RequesterObserver::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json&) {
    return RequesterObserver::get();
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



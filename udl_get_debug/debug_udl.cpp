#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include "common.hpp"
#include <cascade/utils.hpp>

namespace derecho{
namespace cascade{

#define UDL_UUID "8eb917ad-5c51-44de-be0c-2d2803898fd1"
#define UDL_DESC "Get object and get timestamps"

class DebugObserver: public OffCriticalDataPathObserver {
    virtual void operator () (const node_id_t sender,
                              const std::string& key_string,
                              const uint32_t prefix_length,
                              persistent::version_t version,
                              const mutils::ByteRepresentable* const value_ptr,
                              const std::unordered_map<std::string,bool>& outputs,
                              ICascadeContext* ctxt,
                              uint32_t worker_id) override {

        int obj_id = std::stoi(key_string.substr(prefix_length));
        auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);

        // get request
        std::string key = udl_request_path("data");
        global_timestamp_logger.log(TLT_UDLGET(1),typed_ctxt->get_service_client_ref().get_my_id(),obj_id,get_walltime());
        auto req = typed_ctxt->get_service_client_ref().get(key,CURRENT_VERSION,false);
        global_timestamp_logger.log(TLT_UDLGET(2),typed_ctxt->get_service_client_ref().get_my_id(),obj_id,get_walltime());

        // wait future
        for (auto& reply_future:req.get()){
            auto obj = reply_future.second.get();
        }
        global_timestamp_logger.log(TLT_UDLGET(3),typed_ctxt->get_service_client_ref().get_my_id(),obj_id,get_walltime());
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



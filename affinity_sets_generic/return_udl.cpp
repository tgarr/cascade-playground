#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include "common.hpp"

namespace derecho{
namespace cascade{

#define UDL_UUID "b45678de-7a88-4afa-b571-6c87a2f30b01"
#define UDL_DESC "Notify the client that the pipeline is finished."

class ReturnObserver: public OffCriticalDataPathObserver {
    virtual void operator () (const node_id_t sender,
                              const std::string& key_string,
                              const uint32_t prefix_length,
                              persistent::version_t version,
                              const mutils::ByteRepresentable* const value_ptr,
                              const std::unordered_map<std::string,bool>& outputs,
                              ICascadeContext* ctxt,
                              uint32_t worker_id) override {

        // TODO entry logic
        // auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);
        // typed_ctxt->get_service_client_ref().put_and_forget(obj)

        std::cout << "[Return]: I(" << worker_id << ") received an object from sender:" << sender << " with key=" << key_string 
                  << ", matching prefix=" << key_string.substr(0,prefix_length) << std::endl;

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



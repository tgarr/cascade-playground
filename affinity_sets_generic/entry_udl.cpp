#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include "common.hpp"

namespace derecho{
namespace cascade{

#define UDL_UUID "d7bd9797-9e9c-491a-8b17-f74fea8ae52e"
#define UDL_DESC "Entry UDL that decides to which category a given object belongs."

class EntryObserver: public OffCriticalDataPathObserver {
    virtual void operator () (const node_id_t sender,
                              const std::string& key_string,
                              const uint32_t prefix_length,
                              persistent::version_t version,
                              const mutils::ByteRepresentable* const value_ptr,
                              const std::unordered_map<std::string,bool>& outputs,
                              ICascadeContext* ctxt,
                              uint32_t worker_id) override {
        std::cout << "[Entry]: I(" << worker_id << ") received an object from sender:" << sender << " with key=" << key_string 
                  << ", matching prefix=" << key_string.substr(0,prefix_length) << std::endl;

        auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);

        create_pool(typed_ctxt->get_service_client_ref(),"/category/new/test");

        ObjectWithStringKey obj;
        obj.key = "/category/new/test/ID";
        obj.previous_version = INVALID_VERSION;
        obj.previous_version_by_key = INVALID_VERSION;
        obj.blob = Blob(reinterpret_cast<const uint8_t*>(random_buffer(1000)),1000);

        typed_ctxt->get_service_client_ref().put_and_forget(obj);
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<EntryObserver>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }
};

std::shared_ptr<OffCriticalDataPathObserver> EntryObserver::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    // set affinity sets logic
    auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);
    typed_ctxt->get_service_client_ref().set_affinity_set_logic(affinity_logic);

    //initialize observer
    EntryObserver::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json&) {
    return EntryObserver::get();
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


#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include <vector>
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

        // std::cout << "[ENTRY] received: " << key_string << std::endl;
        
        if(key_string.find(OBJ_ENTRY_PATH OBJ_PATH_SEP + std::string("data")) == 0) return;
        auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);

        // find out category
        const auto* const value = dynamic_cast<const ObjectWithStringKey* const>(value_ptr);
        auto hashes = hash_blob(value->blob.bytes,value->blob.size);
        int category = hashes % NUM_CATEGORIES;

        // make a copy
        ObjectWithStringKey obj(*value);
        obj.key = OBJ_NEW_CATEGORY_PATH OBJ_PATH_SEP + std::to_string(category) + OBJ_PATH_SEP + key_string.substr(prefix_length);
        obj.set_previous_version(INVALID_VERSION,INVALID_VERSION);

        // put next object in the pipeline
        // std::cout << "[ENTRY] category: " << category << " | putting: " << obj.key << std::endl;
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


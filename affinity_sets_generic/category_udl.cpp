#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include "common.hpp"

namespace derecho{
namespace cascade{

#define UDL_UUID "8eb917ad-5c51-44de-be0c-2d2803898fd1"
#define UDL_DESC "Perform computation related to a certain category."

class CategoryObserver: public OffCriticalDataPathObserver {
    virtual void operator () (const node_id_t sender,
                              const std::string& key_string,
                              const uint32_t prefix_length,
                              persistent::version_t version,
                              const mutils::ByteRepresentable* const value_ptr,
                              const std::unordered_map<std::string,bool>& outputs,
                              ICascadeContext* ctxt,
                              uint32_t worker_id) override {

        // extract category and id
        std::string key_values = key_string.substr(prefix_length);
        std::string::size_type pos = key_values.find(OBJ_PATH_SEP);
        std::string category = key_values.substr(0,pos);
        std::string obj_id = key_values.substr(pos+1);
        
        std::cout << "[CATEGORY] received: " << key_string << " | category: " << category << " | id: " << obj_id << std::endl;
        auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);

        // compute result
        const auto* const value = dynamic_cast<const ObjectWithStringKey* const>(value_ptr);
        auto result = hash_blob(value->blob.bytes,value->blob.size);

        // get data and compute result
        auto res = typed_ctxt->get_service_client_ref().get(OBJ_DATA_CATEGORY_PATH OBJ_PATH_SEP + category);
        for (auto& reply_future:res.get()){
            auto data_obj = reply_future.second.get();
            result += hash_blob(data_obj.blob.bytes,data_obj.blob.size);
        }

        // put result
        ObjectWithStringKey obj;
        obj.key = OBJ_OUTPUT_CATEGORY_PATH OBJ_PATH_SEP + category + OBJ_PATH_SEP + obj_id;
        obj.previous_version = INVALID_VERSION;
        obj.previous_version_by_key = INVALID_VERSION;
        obj.blob = Blob(reinterpret_cast<const uint8_t*>(&result),sizeof(result));

        std::cout << "[CATEGORY] result: " << result << " | putting: " << obj.key << std::endl;
        typed_ctxt->get_service_client_ref().put_and_forget(obj);
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<CategoryObserver>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }
};

std::shared_ptr<OffCriticalDataPathObserver> CategoryObserver::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    // set affinity sets logic
    auto* typed_ctxt = dynamic_cast<DefaultCascadeContextType*>(ctxt);
    typed_ctxt->get_service_client_ref().set_affinity_set_logic(affinity_logic);

    //initialize observer
    CategoryObserver::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json&) {
    return CategoryObserver::get();
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



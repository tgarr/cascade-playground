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

        // get data
        int num_parts = get_config_int(typed_ctxt->get_service_client_ref(),std::string(OBJ_CONFIG_NUM_ENTRY_PARTS));
        std::vector<derecho::rpc::QueryResults<const derecho::cascade::ObjectWithStringKey>> res;

        // send gets
        for(int i=0;i<num_parts;i++){
            std::string data_key = OBJ_ENTRY_PATH OBJ_PATH_SEP "data_" + std::to_string(i);
            res.push_back(typed_ctxt->get_service_client_ref().get(data_key));
        }

        // get objects
        const uint8_t* blob_bytes[num_parts];
        std::size_t blob_size[num_parts];
        for(int i=0;i<num_parts;i++){
            for (auto& reply_future:res[i].get()){
                auto data_obj = reply_future.second.get();
                blob_bytes[i] = data_obj.blob.bytes;
                blob_size[i] = data_obj.blob.size;
            }
        }

        // hash data after getting all objects
        for(int i=0;i<num_parts;i++){
            hashes += hash_blob(blob_bytes[i],blob_size[i]);
        }
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


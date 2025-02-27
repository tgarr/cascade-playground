
#include <iostream>
#include <vector>
#include <utility>
#include <tuple>
#include <string>
#include <map>
#include <algorithm>
#include <cascade/user_defined_logic_interface.hpp>
#include <cascade/utils.hpp>
#include <cascade/service_client_api.hpp>

namespace derecho{
namespace cascade{

#define UDL_UUID "512e4ccd-7c24-4a23-86cd-17873bbb5576"
#define UDL_DESC "Source UDL"

class SourceUDL: public DefaultOffCriticalDataPathObserver {
    ServiceClientAPI& capi = ServiceClientAPI::get_service_client();

    virtual void ocdpo_handler (
            const node_id_t             sender,
            const std::string&          object_pool_pathname,
            const std::string&          key_string,
            const ObjectWithStringKey&  object,
            const emit_func_t&          emit,
            DefaultCascadeContextType*  typed_ctxt,
            uint32_t                    worker_id) {
    
        std::size_t max_size = 100; // maximum size of the Blob (it can be smaller)

        ObjectWithStringKey obj;
        obj.key = "/cpp/destination/trigger";

        // create a Blob with the new generator that receives the destination type (SAME_PROCESS, SAME_HOST, REMOTE, or NOT_SET) and the destination node id
        obj.blob = Blob([&](uint8_t* buffer,const std::size_t size,const destination_type_t dest_type,const node_id_t dest_node_id){
                std::string mode = "not_set";

                if(dest_type == destination_type_t::SAME_PROCESS){
                    mode = "same_process";
                } else if(dest_type == destination_type_t::SAME_HOST){
                    mode = "same_host";
                } else if(dest_type == destination_type_t::REMOTE){
                    mode = "remote";
                } 

                std::cout << "[CPP source] Generating data in mode '" << mode << "' to be sent to node " << dest_node_id << std::endl;

                // write dummy data to the provided buffer
                uint64_t count = 10;
                uint64_t *uint64_buffer = reinterpret_cast<uint64_t*>(buffer);
                for(uint64_t i=0;i<count;i++){
                    uint64_buffer[i] = i;
                }
                
                // return the number of bytes that were actualy written, which has to be smaller than the max_size given
                return count * sizeof(uint64_t);
            },max_size);
    
        capi.trigger_put(obj);
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<SourceUDL>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }

    void set_config(const nlohmann::json& config){
        // TODO
    }
};

std::shared_ptr<OffCriticalDataPathObserver> SourceUDL::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    //initialize observer
    SourceUDL::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json& config) {
    std::static_pointer_cast<SourceUDL>(SourceUDL::get())->set_config(config);
    return SourceUDL::get();
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



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

#define UDL_UUID "d81617d2-4af2-47e6-9c5a-4758ea17a95c"
#define UDL_DESC "Destination UDL"

class DestinationUDL: public DefaultOffCriticalDataPathObserver {
    ServiceClientAPI& capi = ServiceClientAPI::get_service_client();

    virtual void ocdpo_handler (
            const node_id_t             sender,
            const std::string&          object_pool_pathname,
            const std::string&          key_string,
            const ObjectWithStringKey&  object,
            const emit_func_t&          emit,
            DefaultCascadeContextType*  typed_ctxt,
            uint32_t                    worker_id) {

        std::cout << "[CPP dest] Node " << sender << " trigered me. Data size: " << object.blob.size << std::endl;
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<DestinationUDL>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }

    void set_config(const nlohmann::json& config){
        // TODO
    }
};

std::shared_ptr<OffCriticalDataPathObserver> DestinationUDL::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    //initialize observer
    DestinationUDL::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json& config) {
    std::static_pointer_cast<DestinationUDL>(DestinationUDL::get())->set_config(config);
    return DestinationUDL::get();
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


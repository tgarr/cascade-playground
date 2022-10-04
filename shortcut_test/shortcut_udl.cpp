#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <cascade/utils.hpp>
#include "common.hpp"

namespace derecho{
namespace cascade{

#define UDL_UUID "8eb917ad-5c51-44de-be0c-2d2803898fd1"
#define UDL_DESC "Local get correctness test"

class ShortcutObserver: public DefaultOffCriticalDataPathObserver {
    virtual void ocdpo_handler (
            const node_id_t             sender,
            const std::string&          object_pool_pathname,
            const std::string&          key_string,
            const ObjectWithStringKey&  object,
            const std::function<void(const std::string&, const Blob&)>& emit,
            DefaultCascadeContextType*  typed_ctxt,
            uint32_t                    worker_id) {

        std::string request_key(reinterpret_cast<const char*>(object.blob.bytes));
        auto &capi = typed_ctxt->get_service_client_ref();
            
        auto req = capi.get(request_key);
        for (auto& reply_future:req.get()){
            auto obj = reply_future.second.get();
            std::cout << "[SHORTCUT] Object " << request_key << ": " << reinterpret_cast<const char*>(obj.blob.bytes) << std::endl;
        }
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<ShortcutObserver>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }
};

std::shared_ptr<OffCriticalDataPathObserver> ShortcutObserver::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    //initialize observer
    ShortcutObserver::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json&) {
    return ShortcutObserver::get();
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



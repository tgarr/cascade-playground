#include <cascade/user_defined_logic_interface.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <cascade/utils.hpp>
#include "common.hpp"

namespace derecho{
namespace cascade{

#define UDL_UUID "8eb917ad-5c51-44de-be0c-2d2803898fd1"
#define UDL_DESC "Get object and get timestamps"

class CacheObserver: public DefaultOffCriticalDataPathObserver {
    virtual void ocdpo_handler (
            const node_id_t             sender,
            const std::string&          object_pool_pathname,
            const std::string&          key_string,
            const ObjectWithStringKey&  object,
            const std::function<void(const std::string&, const Blob&)>& emit,
            DefaultCascadeContextType*  typed_ctxt,
            uint32_t                    worker_id) {

        auto cache = typed_ctxt->get_cache_ref();

        if(object_pool_pathname == CLIENT_CACHE_REQUEST){
            std::cout << "[CACHE] Caching object " << object.key << std::endl;
            cache.put(object);
        }
        else if(object_pool_pathname == CLIENT_CACHE_CHECK){
            std::string request_key(reinterpret_cast<const char*>(object.blob.bytes));
            bool is_cached = cache.is_cached(request_key);
            std::cout << "[CACHE] Cache status for object " << request_key << ": " << is_cached << std::endl;
        }
        else {
            std::cout << "[CACHE] Nothing to do." << std::endl;
        }
    }

    static std::shared_ptr<OffCriticalDataPathObserver> ocdpo_ptr;
public:
    static void initialize() {
        if(!ocdpo_ptr) {
            ocdpo_ptr = std::make_shared<CacheObserver>();
        }
    }
    static auto get() {
        return ocdpo_ptr;
    }
};

std::shared_ptr<OffCriticalDataPathObserver> CacheObserver::ocdpo_ptr;

void initialize(ICascadeContext* ctxt) {
    //initialize observer
    CacheObserver::initialize();
}

std::shared_ptr<OffCriticalDataPathObserver> get_observer(
        ICascadeContext*,const nlohmann::json&) {
    return CacheObserver::get();
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



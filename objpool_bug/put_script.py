
from derecho.cascade.external_client import ServiceClientAPI

capi = ServiceClientAPI()

for i in range(100):
    capi.create_object_pool(f"/test{i}","VolatileCascadeStoreWithStringKey",0)
    capi.put(f"/test{i}/{i}",b"test")


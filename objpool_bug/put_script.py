
import time
import numpy as np
from derecho.cascade.external_client import ServiceClientAPI

capi = ServiceClientAPI()
c = []
p = []

for i in range(100):
    start = time.time()
    capi.create_object_pool(f"/test{i}","VolatileCascadeStoreWithStringKey",0)
    end1 = time.time()
    capi.put(f"/test{i}/{i}",b"test")
    end2 = time.time()

    c.append((end1-start)*1000)
    p.append((end2-end1)*1000)

print(f"create: {np.mean(c):.2f}")
print(f"put: {np.mean(p):.2f}")


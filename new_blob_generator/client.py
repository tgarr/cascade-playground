#!/usr/bin/env python3

import os
import sys
import time
from derecho.cascade.external_client import ServiceClientAPI

SUBGROUP_TYPES = {
    "VCSS": "VolatileCascadeStoreWithStringKey",
    "PCSS": "PersistentCascadeStoreWithStringKey",
    "TCSS": "TriggerCascadeNoStoreWithStringKey"
}

def usage(argv):
    print(f"usage: {argv[0]} <cpp|python>")

def main(argv):
    if len(argv) < 2:
        usage(argv)
        return
    
    mode = argv[1]
    if mode not in ("cpp","python"):
        usage(argv)
        return

    print("Connecting to Cascade service ...")
    capi = ServiceClientAPI()

    # create object pools
    print("Creating object pools ...")
    source_pathname = f"/{mode}/source"
    dest_pathname = f"/{mode}/destination"
    capi.create_object_pool(source_pathname,SUBGROUP_TYPES["VCSS"],0)
    time.sleep(1) # XXX BUG: can't create pools too fast
    capi.create_object_pool(dest_pathname,SUBGROUP_TYPES["VCSS"],1)
    time.sleep(1) # XXX BUG: can't create pools too fast

    # trigger source UDL
    print(f"Triggering {mode} source UDL ...")
    key = f"{source_pathname}/trigger"
    capi.put(key,b"trigger",trigger=True)

    print("Done!")

if __name__ == "__main__":
    main(sys.argv)


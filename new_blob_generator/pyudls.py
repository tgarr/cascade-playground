
import numpy as np
import json
import cascade_context
from derecho.cascade.member_client import ServiceClientAPI
from derecho.cascade.udl import UserDefinedLogic

class SourceUDL(UserDefinedLogic):
    def __init__(self,conf_str):
        super(SourceUDL,self).__init__(conf_str)
        self.conf = json.loads(conf_str)
        self.capi = ServiceClientAPI()

    def ocdpo_handler(self,**kwargs):
        # request info
        sender = kwargs["sender"]
        key = kwargs["key"]
        pathname = kwargs["pathname"]
        blob = kwargs["blob"]
        worker_id = kwargs["worker_id"]

        def data_generator(max_size,dest_type,dest_node_id):
            mode = "not_set"
            if dest_type == 1:
                mode = "same_process"
            elif dest_type == 2:
                mode = "same_host"
            elif dest_type == 3:
                mode = "remote"
            
            print(f"[Python source] Generation data in mode '{mode}' to be sent to node {dest_node_id}")
            return np.array([1,2,3],dtype=int).tobytes()

        self.capi.put("/python/destination/trigger",b'',generator=data_generator,generator_max_size=100,trigger=True)

        
    def __del__(self):
        pass

class DestinationUDL(UserDefinedLogic):
    def __init__(self,conf_str):
        super(DestinationUDL,self).__init__(conf_str)
        self.conf = json.loads(conf_str)
        self.capi = ServiceClientAPI()

    def ocdpo_handler(self,**kwargs):
        # request info
        sender = kwargs["sender"]
        key = kwargs["key"]
        pathname = kwargs["pathname"]
        blob = kwargs["blob"]
        worker_id = kwargs["worker_id"]

        array = blob[0:24].view(dtype=int)
        print(f"[Python dest] Node {sender} trigered me. Data {array}")

    def __del__(self):
        pass


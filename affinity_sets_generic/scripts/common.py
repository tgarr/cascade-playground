
import numpy as np

# default values
OBJECT_SIZE = 1000000
OBJECT_RATE = 50

ENTRY_PART_SIZE = 1000000
NUM_ENTRY_PARTS = 5

DATA_PART_SIZE = 1000000
NUM_DATA_PARTS = 100

NUM_SHARDS = 5
NUM_CATEGORIES = 10
AFFINITY_LOGIC = 2

SKIP_OBJECTS = 500
BENCHMARK_TIME = 1500
AFFINITY_LOGIC_LABEL = {0:"no grouping",1:"entry + category",2:"only category"}

# plot config
BAR_WIDTH = 0.4

def extract_raw_data(fname):
    data = {}

    f = open(fname,"r")

    for header in f:
        fields = header.split(" ")

        client_id = int(fields[0])
        object_size = int(fields[1])
        object_rate = int(fields[2])
        entry_part_size = int(fields[3])
        num_entry_parts = int(fields[4])
        data_part_size = int(fields[5])
        num_data_parts = int(fields[6])
        num_objects = int(fields[7])
        affinity_logic = int(fields[8])
        num_shards = int(fields[9])
        num_categories = int(fields[10])
        duration = int(fields[11])
        mode = fields[12]

        i = 0
        for sample in f:
            values = sample.split(" ")
            obj_id = int(values[0])
            if obj_id >= SKIP_OBJECTS:
                latency = int(values[1])
                timestamp = int(values[2])
                category = int(values[3])
                node_id = int(values[4])

                if latency > 0:
                    exp_config = (object_size,object_rate,entry_part_size,num_entry_parts,data_part_size,num_data_parts,num_shards,num_categories)
                    if affinity_logic not in data: 
                        data[affinity_logic] = {}
                    if exp_config not in data[affinity_logic]:
                        data[affinity_logic][exp_config] = ([],[],[])

                    data[affinity_logic][exp_config][0].append(latency)
                    data[affinity_logic][exp_config][1].append(category)
                    data[affinity_logic][exp_config][2].append(node_id)

            i += 1
            if i >= num_objects:
                break

    f.close()

    return data

def get_values(data,index,max_value=99999999):
    values = []
    logics = sorted(list(data.keys()))
    exp_config = None

    for l in logics:
        for conf in data[l]:
            exp_config = conf
            v = conf[index]
            if v not in values and v <= max_value: values.append(v)
    values.sort()

    avg = {}
    std = {}
    for l in logics:
        avg[l] = []
        std[l] = []

        for v in values:
            conf = list(exp_config)
            conf[index] = v
            conf = tuple(conf)
            if conf in data[l]:
                varray = np.array(data[l][conf][0])
                avg[l].append(np.mean(varray))
                std[l].append(np.std(varray))
            else:
                avg[l].append(0)
                std[l].append(0)
    
    return values,avg,std


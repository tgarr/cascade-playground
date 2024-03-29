
import numpy as np
import array

TAG_UDL_GET_START = 8001
TAG_UDL_GET_END = 8002
TAG_UDL_WAIT_END = 8003

DEFAULT_SAMPLES = 110000
DEFAULT_SKIP = 10000

# plot config
BAR_WIDTH = 0.4

def extract_data(size,mode,samples=None,skip=None):
    if samples == None:
        samples = DEFAULT_SAMPLES
        total = DEFAULT_SAMPLES - DEFAULT_SKIP
        skip = DEFAULT_SKIP
    else:
        if skip == None:
            skip = 0
        elif type(skip) == int:
            total -= skip
        elif type(skip) == float:
            skip = int(total * skip)
            total = total - skip

    fname = f"raw/{size}_{samples}.{mode}"

    timestamps = dict({
        TAG_UDL_GET_START: array.array('q',[0]*total),
        TAG_UDL_GET_END: array.array('q',[0]*total),
        TAG_UDL_WAIT_END: array.array('q',[0]*total),
        })
    
    with open(fname,"r") as f:
        for line in f:
            fields = line.split()
            tag = int(fields[0])
            obj_id = int(fields[2])
            value = int(fields[3])

            if obj_id < skip: continue

            if tag in timestamps:
                timestamps[tag][obj_id-skip] = value

    udl_get_start = np.array(timestamps[TAG_UDL_GET_START])/1e3
    udl_get_end = np.array(timestamps[TAG_UDL_GET_END])/1e3
    udl_wait_end = np.array(timestamps[TAG_UDL_WAIT_END])/1e3

    udl_get_lat = udl_get_end - udl_get_start
    udl_wait_lat = udl_wait_end - udl_get_end
    udl_total_lat = udl_get_lat + udl_wait_lat

    udl_total_time = udl_wait_end[-1] - udl_get_start[0]
    throughput = len(udl_get_lat) / float(udl_total_time) * 1000000

    return (udl_get_lat,udl_wait_lat,udl_total_lat,throughput)


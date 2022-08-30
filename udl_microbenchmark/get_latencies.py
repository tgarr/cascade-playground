
import sys
import numpy as np
import array

SKIP = 100
TOTAL = 1100

TAG_UDL_GET_START = 8001
TAG_UDL_GET_END = 8002
TAG_UDL_WAIT_END = 8003

if __name__ == "__main__":
    fname = sys.argv[1]
    if len(argv) > 2:
        total = int(argv[2]) - SKIP
    else:
        total = TOTAL - SKIP

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

            if obj_id < SKIP: continue

            if tag in timestamps:
                timestamps[tag][obj_id-SKIP] = value

    udl_get_start = np.array(timestamps[TAG_UDL_GET_START])/1e3
    udl_get_end = np.array(timestamps[TAG_UDL_GET_END])/1e3
    udl_wait_end = np.array(timestamps[TAG_UDL_WAIT_END])/1e3

    udl_get_lat = udl_get_end - udl_get_start
    udl_wait_lat = udl_wait_end - udl_get_end
    udl_complete_lat = udl_wait_end[-1] - udl_get_start[0]
    throughput = len(udl_get_lat) / float(udl_complete_lat) * 1000000

    #print(f"num samples: {len(udl_get_lat)}")
    print(f"udl_get_lat: {np.mean(udl_get_lat):.2f} / {np.std(udl_get_lat):.2f}")
    print(f"udl_wait_lat: {np.mean(udl_wait_lat):.2f} / {np.std(udl_wait_lat):.2f}")
    #print(f"udl_complete_lat: {udl_complete_lat}")
    print(f"throughput: {throughput:.2f}")


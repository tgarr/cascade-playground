#!/usr/bin/env python3

import sys
import numpy as np
import array

SKIP = 1000
TOTAL = 11000

TAG0 = 98760
TAG1 = TAG0 + 1
TAG2 = TAG1 + 1
TAG3 = TAG2 + 1
TAG4 = TAG3 + 1
TAG5 = TAG4 + 1
TAG6 = TAG5 + 1

if __name__ == "__main__":
    fname = sys.argv[1]
    if len(sys.argv) > 2:
        total = int(sys.argv[2])
        skip = int(total * 0.1)
        total = total - skip
    else:
        skip = SKIP
        total = TOTAL - SKIP

    timestamps = dict({
        TAG0: array.array('q',[0]*total),
        TAG1: array.array('q',[0]*total),
        TAG2: array.array('q',[0]*total),
        TAG3: array.array('q',[0]*total),
        TAG4: array.array('q',[0]*total),
        TAG5: array.array('q',[0]*total),
        TAG6: array.array('q',[0]*total),
        })
   
    count = {}
    for t in [TAG0,TAG1,TAG2,TAG3,TAG4,TAG5]: count[t] = 0
    with open(fname,"r") as f:
        for line in f:
            fields = line.split()
            tag = int(fields[0])
            obj_id = int(fields[2])
            value = int(fields[3])

            if tag in timestamps:
                if count[tag] >= skip:
                    timestamps[tag][count[tag]-skip] = value
                count[tag] += 1

    time0 = np.array(timestamps[TAG0])/1e3
    time1 = np.array(timestamps[TAG1])/1e3
    time2 = np.array(timestamps[TAG2])/1e3
    time3 = np.array(timestamps[TAG3])/1e3
    time4 = np.array(timestamps[TAG4])/1e3
    time5 = np.array(timestamps[TAG5])/1e3
    time6 = np.array(timestamps[TAG5])/1e3
    
    interval0 = time1 - time0
    interval1 = time2 - time1
    interval2 = time3 - time2
    interval3 = time4 - time3
    interval4 = time5 - time4
    interval5 = time6 - time5
    total = time6 - time0

    print(f"interval0: {np.mean(interval0):.2f} / {np.std(interval0):.2f}")
    print(f"interval1: {np.mean(interval1):.2f} / {np.std(interval1):.2f}")
    print(f"interval2: {np.mean(interval2):.2f} / {np.std(interval2):.2f}")
    print(f"interval3: {np.mean(interval3):.2f} / {np.std(interval3):.2f}")
    print(f"interval4: {np.mean(interval4):.2f} / {np.std(interval4):.2f}")
    print(f"interval5: {np.mean(interval5):.2f} / {np.std(interval5):.2f}")


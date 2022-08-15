
import sys
import numpy as np

f = open(sys.argv[1],"r")
tag = sys.argv[2]
PARTS = int(sys.argv[3])

values = []
for line in f.readlines():
    if not line.startswith(tag): continue

    try:
        d = line.strip().split()

        if line.startswith("RPC GET"): l = int(d[2])
        elif line.startswith("obj "): l = int(d[7])
        else: l = int(d[1])

        values.append(l)
    except:
        pass

f.close()

avg = sum(values) / float(len(values))
print("num:\t\t%d (%d)" % (len(values),len(values)/PARTS))
print("average:\t%.2f (%.2f)" % (avg,avg*PARTS))
print("min:\t\t%d (%d)" % (min(values),PARTS*min(values)))
print("max:\t\t%d (%d)" % (max(values),PARTS*max(values)))
print("std:\t\t%.2f" % np.std(np.array(values)))



import sys

SKIP = 500

f = open(sys.argv[1],"r")

values = []
for line in f.readlines():
    d = line.strip().split()
    obj_id = int(d[1].split("_")[1])
    if obj_id < SKIP: continue

    l = int(d[7])
    values.append(l)

f.close()

avg = sum(values) / float(len(values))
print("num:\t\t%d" % len(values))
print("average:\t%.2f" % avg)
print("min:\t\t%d " % min(values))
print("max:\t\t%d" % max(values))


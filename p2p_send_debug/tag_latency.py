
import sys
import numpy as np

tag = sys.argv[2]
f = open(sys.argv[1],"r")

count = 0
values = []
for line in f.readlines():
    d = line.split()
    if len(d) != 2: continue

    try:
        if d[0] != tag: continue
        values.append(int(d[1]))
        count += 1
    except:
        pass

f.close()

varray = np.array(values)

print("%s (%d samples):" % (tag,count))
print("  avg: %.2f" % np.mean(varray))
print("  std: %.2f" % np.std(varray))
print("  min: %d" % np.min(varray))
print("  max: %d" % np.max(varray))



import sys
import numpy as np

OBJ_SKIP = 500

f = open(sys.argv[1],"r")

count = 0
p2p_latencies = []
wait_latencies = []

for line in f.readlines():
    values = line.split()

    try:
        if int(values[1]) < OBJ_SKIP: continue
    except:
        continue

    for v in values[2:]:
        try:
            if values[0] == "P2P_SEND": p2p_latencies.append(int(v))
            elif values[0] == "WAIT": wait_latencies.append(int(v))
            count += 1
        except:
            pass

count = count/2
p2p_array = np.array(p2p_latencies)
wait_array = np.array(wait_latencies)

print("P2P_SEND (%d samples):" % count)
print("  avg: %.2f" % np.mean(p2p_array))
print("  std: %.2f" % np.std(p2p_array))
print("  min: %d" % np.min(p2p_array))
print("  max: %d" % np.max(p2p_array))

print("WAIT (%d samples):" % count)
print("  avg: %.2f" % np.mean(wait_array))
print("  std: %.2f" % np.std(wait_array))
print("  min: %d" % np.min(wait_array))
print("  max: %d" % np.max(wait_array))

f.close()


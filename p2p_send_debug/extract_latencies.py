
import sys
import numpy as np

OBJ_SKIP = 500

f = open(sys.argv[1],"r")
num_parts = int(sys.argv[2])

p2p_latencies = []
wait_latencies = []
g_latencies = []

for line in f.readlines():
    values = line.split()
    if len(values) != num_parts+2 and len(values) != 3: continue

    try:
        if int(values[1]) < OBJ_SKIP: continue
    except:
        continue

    for v in values[2:]:
        try:
            if values[0] == "P2P_SEND": p2p_latencies.append(int(v))
            elif values[0] == "WAIT": wait_latencies.append(int(v))
            elif values[0] == "GLOBAL": g_latencies.append(int(v))
        except:
            pass

p2p_array = np.array(p2p_latencies)
wait_array = np.array(wait_latencies)
g_array = np.array(g_latencies)

print("GLOBAL (%d samples):" % len(g_latencies))
print("  avg: %.2f" % np.mean(g_array))
print("  std: %.2f" % np.std(g_array))
print("  min: %d" % np.min(g_array))
print("  max: %d" % np.max(g_array))

print("P2P_SEND (%d samples):" % len(p2p_latencies))
print("  avg: %.2f" % np.mean(p2p_array))
print("  std: %.2f" % np.std(p2p_array))
print("  min: %d" % np.min(p2p_array))
print("  max: %d" % np.max(p2p_array))

print("WAIT (%d samples):" % len(wait_latencies))
print("  avg: %.2f" % np.mean(wait_array))
print("  std: %.2f" % np.std(wait_array))
print("  min: %d" % np.min(wait_array))
print("  max: %d" % np.max(wait_array))

f.close()


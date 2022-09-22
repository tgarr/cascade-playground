
import sys
import numpy as np
import matplotlib.pyplot as plt
from common import *

def main(argv):
    size = argv[1]
    rates = argv[2:]
    modes = ["local","remote"]

    avg = {}
    std = {}
    thr = {}
    for m in modes:
        avg[m] = []
        std[m] = []
        thr[m] = []
        for r in rates:
            udl_get_lat,udl_wait_lat,udl_total_lat,throughput = extract_data(r,size,m,skip=0.1)
            avg[m].append(np.average(udl_total_lat))
            std[m].append(np.std(udl_total_lat))
            thr[m].append(int(throughput))

    tw = float(len(modes) * BAR_WIDTH)
    step = tw+BAR_WIDTH
    x = np.arange(0,(len(rates)-1)*step+0.1,step)
    start = - (BAR_WIDTH/2)*(len(modes)-1)
    i = 0
    for m in modes:
        offset = start + BAR_WIDTH*i  
        plt.bar(x + offset,avg[m],yerr=std[m],width=BAR_WIDTH,label=m)
        i += 1


    xt = []
    i = 0
    while i < len(rates):
        r = rates[i]
        t = thr['local'][i]
        xt.append(str(r) + "/" + str(t))
        i += 1

    plt.ylabel('Microseconds')
    plt.xlabel('Request rate / actual throughput (objects/s)')
    plt.title("Latency to get payloads of size " + size)
    plt.xticks(x,xt)
    #plt.yticks(np.arange(0,70000,5000))
    plt.legend()
    
    plt.savefig("rates_" + size + ".png",dpi=200)

if __name__ == "__main__":
    main(sys.argv)



import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import ticker
from common import *

# np.percentile(array,p)
# np.median(array)
# np.mean(array)
# np.std(array)
# box plot?

LABEL = {
        "local": "local get",
        "remote": "remote get"
    }

def main(argv):
    sizes = argv[1:]
    modes = ["local","remote"]

    # get data
    avg = {}
    std = {}
    #med = {}
    #p95 = {}
    #p99 = {}
    #thr = {}
    #cdf = {}
    #pdf = {}
    for m in modes:
        avg[m] = []
        std[m] = []
        for sz in sizes:
            _,_,data,_ = extract_data(sz,m)
            avg[m].append(np.mean(data))
            std[m].append(np.std(data))

    # plot
    fig1, ax1 = plt.subplots()
    
    tw = float(len(modes) * BAR_WIDTH)
    step = tw+BAR_WIDTH
    x = np.arange(0,(len(sizes)-1)*step+0.1,step)
    start = - (BAR_WIDTH/2)*(len(modes)-1)
    i = 0
    for m in modes:
        offset = start + BAR_WIDTH*i  
        ax1.bar(x + offset,avg[m],yerr=std[m],width=BAR_WIDTH,label=LABEL[m],zorder=2)
        i += 1

    ax1.set_ylabel('Latency in microseconds (log scale)')
    ax1.set_xlabel('Object size in KB')
    ax1.legend(loc='upper left')

    ax1.set_xticks(x,[s[:-1] for s in sizes])
    ax1.set_yscale('log')
    f1 = ticker.FormatStrFormatter("%d")
    f2 = ticker.ScalarFormatter()
    f2.set_scientific(False)
    ax1.get_yaxis().set_major_formatter(f1)
    ax1.get_yaxis().set_minor_formatter(f2)
    ax1.get_yaxis().set_minor_locator(plt.FixedLocator([5,7,15,30,50,150,300,500,1900]))
    
    ax1.grid(axis='y',which='both',alpha=0.8,ls='--',zorder=1)
    
    plt.savefig("plot_latency_sizes.png",dpi=200)
    #plt.show()

if __name__ == "__main__":
    main(sys.argv)


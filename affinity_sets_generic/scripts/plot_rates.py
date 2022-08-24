
import sys
import numpy as np
import matplotlib.pyplot as plt
from common import *

RATES = [10,20,30,40,50,60,70,80,90,100,110,120,130,140]
RATES = [10,20,30,40,50,60]

def main(argv):
    fname = argv[1]
    data = extract_raw_data(fname)
    logics = sorted(list(data.keys()))

    avg = {}
    std = {} 
    for l in logics: 
        avg[l] = []
        std[l] = []

        for r in RATES:
            exp_config = (OBJECT_SIZE,r,ENTRY_PART_SIZE,NUM_ENTRY_PARTS,DATA_PART_SIZE,NUM_DATA_PARTS,NUM_SHARDS,NUM_CATEGORIES)
            if exp_config in data[l]:
                varray = np.array(data[l][exp_config][0])
                avg[l].append(np.mean(varray))
                std[l].append(np.std(varray))
            else:
                avg[l].append(0)
                std[l].append(0)

    tw = float(len(logics) * BAR_WIDTH)
    step = tw+BAR_WIDTH
    x = np.arange(0,(len(RATES)-1)*step+0.1,step)
    start = - (BAR_WIDTH/2)*(len(logics)-1)
    i = 0
    for l in logics:
        offset = start + BAR_WIDTH*i  
        plt.bar(x + offset,avg[l],yerr=std[l],width=BAR_WIDTH,label=AFFINITY_LOGIC_LABEL[l])
        i += 1

    plt.ylabel('Microseconds')
    plt.title('Objects / seconds')
    plt.xticks(x,RATES)
    #plt.yticks(np.arange(0,70000,5000))
    plt.legend()
    
    plt.show()

if __name__ == "__main__":
    main(sys.argv)


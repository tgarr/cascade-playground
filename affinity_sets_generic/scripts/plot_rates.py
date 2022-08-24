
import sys
import numpy as np
import matplotlib.pyplot as plt
from common import *

def main(argv):
    fname = argv[1]
    max_value = 99999999
    if len(argv) > 2:
        max_value = int(argv[2])
   
    data = extract_raw_data(fname)
    rates,avg,std = get_values(data,1,max_value)
    logics = sorted(list(avg.keys()))

    tw = float(len(logics) * BAR_WIDTH)
    step = tw+BAR_WIDTH
    x = np.arange(0,(len(rates)-1)*step+0.1,step)
    start = - (BAR_WIDTH/2)*(len(logics)-1)
    i = 0
    for l in logics:
        offset = start + BAR_WIDTH*i  
        plt.bar(x + offset,avg[l],yerr=std[l],width=BAR_WIDTH,label=AFFINITY_LOGIC_LABEL[l])
        i += 1

    plt.ylabel('Microseconds')
    plt.title('Objects / seconds')
    plt.xticks(x,rates)
    #plt.yticks(np.arange(0,70000,5000))
    plt.legend()
    
    plt.show()

if __name__ == "__main__":
    main(sys.argv)


import uproot as ur
import numpy as np
import matplotlib.pyplot as plt
import sys 

fname = sys.argv[1]

moduleid, fiberid, sipmid, xpos, ypos = np.loadtxt("mapfile.txt").T

sipmid = np.uint32(sipmid)

fileHandle = ur.open(fname)

integral = np.array(fileHandle["Ntuples/SiPM"]["Integral"])

for i in range(integral.shape[-1]):
    evtIntegral = integral[i]
    plt.scatter(xpos, ypos, s=1, c="r")
    plt.show()

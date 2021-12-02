import uproot
import matplotlib.pyplot as plt
import mplhep as hep
import sys
import numpy as np


def idxToPos():
    ROW = []
    COL = []
    for idx in range(3*3*16*20):
        module = idx // (20 * 16)
        idxInModule = idx % (20*16)
        moduleRow = module // 3
        moduleCol = module % 3
        rowInModule = idxInModule // 20
        colInModule = idxInModule % 20
        row = 48 - 16 * (moduleRow + 1) + rowInModule
        col = colInModule + 20 * moduleCol
        ROW.append(row)
        COL.append(col)
    return np.array(ROW), np.array(COL)


rowmap, colmap = idxToPos()
NMODULES = 3
ROWS = 20 * NMODULES
COLS = 16 * NMODULES

if "-f" in sys.argv:
    fname = sys.argv[sys.argv.index("-f")+1]
else:
    print("Missing filename")
    exit()

file = uproot.open(fname)
nEvents = file["Ntuples/Energy/Total"].num_entries

integrals = np.array(file["Ntuples/SiPM/Integral"].array())
integrals = integrals.reshape(nEvents, ROWS, COLS)

fig, ax = plt.subplots()
for i in range(nEvents):
    ax.matshow(integrals[i, rowmap, colmap])
    plt.pause(0.25)
    ax.cla()

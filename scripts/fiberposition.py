import numpy as np


moduleid, modulex, moduley = np.loadtxt("modulepos.txt").T
fiberid, fiberx, fibery = np.loadtxt("fiberpos.txt").T

moduleid = np.uint32(moduleid)
fiberid = np.uint32(fiberid)

f = open("mapfile.txt","w")

for i,mid in enumerate(moduleid):
    for j,fid in enumerate(fiberid):
        sipmid = (mid*16*32) + fid
        sipmposx = fiberx[j] + modulex[i]
        sipmposy = fibery[j] + moduley[i]

        f.write(f"{mid:d} {fid:d} {sipmid:d} {sipmposx:.2f} {sipmposy:.2f}\n")

f.close()

import CalRun
import numpy as np

tcalshift = 300
dtrue = 1.
A = 1.0
c = CalRun.CalRun (N=20, pf=False, sky=False, cal=True, notch=True, verbose=False, drift=dtrue, A=A, 
                 calfile = "data/samples/calib_filt.txt", cal_shift = tcalshift,seed=int(A*999),force=False)
c.get_sig(tcalshift,dtrue,"data/samples/calib_filt.txt")
print (np.real(c.rbeam))


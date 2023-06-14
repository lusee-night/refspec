
import numpy as np
import refspec
import lusee

Nsec = 0.1
cfg = refspec.cppyy.gbl.SpecConfig()
sampling_rate = cfg.sampling_rate
block_size = cfg.Nfft ## irrelevant here
Nsamples = int(Nsec*sampling_rate)
Nchannels = 1
Nblocks = Nsamples//block_size+1
seed =1 
f=np.linspace(0,52,1000)
P = 1000+f*f

a=refspec.CorrelatedSpecSource (f,sampling_rate, block_size, Nchannels, Nblocks+1, False, False, seed, True)
for i,f_ in enumerate(f):
    a._set_internal_C(i,0,0,complex(np.sqrt(P[i])))

a._go_internal(f,seed)

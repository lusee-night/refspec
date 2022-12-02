#!/usr/bin/env python3


import sys
 
# appending a path
sys.path.append('../src')

try:
    import refspec
except:
    print("Error importing main refspec module")

print("refspec docstring:", refspec.__doc__)

cfg = refspec.SpecConfig()

cfg.Ntaps           = 8
cfg.Nchannels       = 1
cfg.Average1Size    = 64
cfg.Average2Size    = 1

fundamental         = cfg.fundamental_frequency()

blocks              = cfg.AverageSize()+2*cfg.Ntaps
Ampl                = 10.0;
noiseA              = 0.0;

signal = refspec.SignalGenerator(cfg.Nfft, cfg.Nchannels, blocks, 10e6, cfg.sampling_rate, Ampl, noiseA)

output = refspec.SpecOutput(cfg)

# print(output.Nchannels, output.Nspec, output.Nbins)



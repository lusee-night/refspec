#!/usr/bin/env python3.10


import sys
 
sys.path.append('../')

try:
    import refspec
except:
    print("Error importing main refspec module")

print("Module 'refspec' docstring:", refspec.__doc__)

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

spectrometer = refspec.RefSpectrometer(signal, cfg)
spectrometer.run(output)

for i in range(1, cfg.Nbins()): print(fundamental*i/1e6, output.get_avg_pspec(0, i))



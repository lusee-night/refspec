#!/usr/bin/env python3.10
import cppyy
from cppyy import ll

headers = [
    'CombSource.h',
    'FileStreamSource.h',
    'pfb.h',
    'PowerSpecSource.h',
    'RefSpectrometer.h',
    'SignalCombiner.h',
    'SignalGenerator.h',
    'SignalSource.h',
    'SpecConfig.h',
    'SpecOutput.h',
    'WhiteNoise.h',
]

for header in headers:
    print(f'''Loading {header}''')
    cppyy.include(header)

cppyy.load_library('refspec')

from cppyy.gbl import SpecConfig, SignalGenerator, SpecOutput, RefSpectrometer

# -----------------------------------------------------------------------------------------

cfg = SpecConfig()

cfg.Ntaps           = 8
cfg.Nchannels       = 1
cfg.Average1Size    = 64
cfg.Average2Size    = 1

fundamental         = cfg.fundamental_frequency()

blocks              = cfg.AverageSize()+2*cfg.Ntaps
Ampl                = 10.0;
noiseA              = 0.0;

signal = SignalGenerator(cfg.Nfft, cfg.Nchannels, blocks, 10e6, cfg.sampling_rate, Ampl, noiseA)
output = SpecOutput(cfg)

spectrometer = RefSpectrometer(signal, cfg)
spectrometer.run(output)

for i in range(1, cfg.Nbins()): print(fundamental*i/1e6, output.get_avg_pspec(0, i))

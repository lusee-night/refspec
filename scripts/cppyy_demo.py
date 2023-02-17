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

from cppyy.gbl import SpecConfig

# dir(SpecConfig)

cfg = SpecConfig()

cfg.Ntaps           = 8
cfg.Nchannels       = 1
cfg.Average1Size    = 64
cfg.Average2Size    = 1

fundamental         = cfg.fundamental_frequency()

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

from cppyy.gbl import RefSpectrometer, PowerSpecSource, CombSource, SpecConfig, SignalGenerator, SignalCombiner, SignalSource, SpecOutput
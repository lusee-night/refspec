import cppyy
from cppyy import ll

headers = [
    'CombSource.h',
    'FileStreamSource.h',
    'FileStreamSink.h',
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
    try:
        cppyy.include(header)
    except:
        print(f'''Error loading {header}, exiting''')
        exit(-1)

cppyy.load_library('refspec')

from cppyy.gbl import RefSpectrometer, PowerSpecSource, CombSource, SpecConfig, SignalGenerator, SignalCombiner, SignalSource, SpecOutput, FileStreamSource, FileStreamSink, WhiteNoise
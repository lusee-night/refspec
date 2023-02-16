import cppyy
from cppyy import ll
cppyy.include('CombSource.h')
cppyy.include('FileStreamSource.h')
cppyy.include('pfb.h')
cppyy.include('PowerSpecSource.h')
cppyy.include('RefSpectrometer.h')
cppyy.include('SignalCombiner.h')
cppyy.include('SignalGenerator.h')
cppyy.include('SignalSource.h')
cppyy.include('SpecConfig.h')
cppyy.include('SpecOutput.h')
cppyy.include('WhiteNoise.h')
cppyy.load_library('mylib')
from cppyy.gbl import SpecConfig
dir(SpecConfig)
cfg = refspec.SpecConfig()
cfg = SpecConfig()

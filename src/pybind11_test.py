#!/usr/bin/env python3

# Testing the refspec Python package obtained using pybind11


from refspec import SpecConfig, SpecOutput, SignalGenerator

cfg =   SpecConfig()
so  =   SpecOutput(cfg)

print(so.get_Nfft())
print(so.get_mode())

print("*** SpecConfig ***")
print(f'''sampling_rate: {cfg.sampling_rate}, Nfft: {cfg.Nfft}, Ntaps: {cfg.Ntaps}, AverageSize: {cfg.AverageSize}''')


# Further testing taken from "simple_demo"

cfg.Ntaps           = 8
cfg.Nchannels       = 1
cfg.AverageSize     = 64

fundamental         = cfg.fundamental_frequency()

blocks              = cfg.AverageSize+2*cfg.Ntaps
Ampl                = 10
noiseA              = 0.0


signal = SignalGenerator(cfg.Nfft, cfg.Nchannels, blocks, 10e6, cfg.sampling_rate, Ampl, noiseA)


exit(0)


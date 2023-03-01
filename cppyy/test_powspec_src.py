#!/usr/bin/env python3.10

### CPPYY ###

from refspec import SpecConfig, PowerSpecSource, SpecOutput, RefSpectrometer

# -----------------------------------------------------------------------------------------

cfg = SpecConfig()

cfg.Nfft            = 2048
cfg.Ntaps           = 8
cfg.Nchannels       = 1
cfg.Average1Size    = 64
cfg.Average2Size    = 500
cfg.sampling_rate   = 1.0e8

fundamental         = cfg.fundamental_frequency()

block_size   = cfg.Nfft
Nvar         = 10
Nblocks_gen  = cfg.MinGenSize()+Nvar

print(f'''Fundamental frequency: {fundamental}, Nblocks_gen {Nblocks_gen}''')

source = PowerSpecSource("data/Pk/Pk_wnoise.txt", cfg.sampling_rate, block_size, cfg.Nchannels, Nblocks_gen, False, False)

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

measured_var = 0.0

block_size   = cfg.Nfft
Nvar         = 10
Nblocks_gen  = cfg.MinGenSize()+Nvar

print(f'''Fundamental frequency: {fundamental}, Nblocks_gen {Nblocks_gen}''')

source = PowerSpecSource("data/Pk/Pk_wnoise.txt", cfg.sampling_rate, block_size, cfg.Nchannels, Nblocks_gen, False, False)

source.init_internal(1)
print(f'''Internal storage initialized (true/false): {source.has_internal()}, number of internal elements: {source.get_Ninternal()}''')


cnt = 0
for i in range(Nvar):
    print(f'''Nvar index: {i}''')
    source.next_block()
    for n in range(block_size):
        measured_var+=source.get_internal(0,n)*source.get_internal(0,n)
        cnt+=1

measured_var /= cnt
print(f'''measured_var: {measured_var}''')


output = SpecOutput(cfg)
print('Created the SpecOutput object')

spectrometer = RefSpectrometer(source, cfg)
print('Created the refspec object')

spectrometer.run(output)

print(f'''Nradiometer: {output.Nradiometer}''')



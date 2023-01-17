#!/usr/bin/env python3.10

import sys
 
sys.path.append('./')

try:
    import refspec
except:
    print("Error importing main refspec module")

print("Module 'refspec' docstring:", refspec.__doc__)

cfg = refspec.SpecConfig()

cfg.Nfft            = 2048
cfg.Ntaps           = 8
cfg.Nchannels       = 1
cfg.Average1Size    = 64
cfg.Average2Size    = 500
cfg.sampling_rate   = 1.0e8

fundamental         = cfg.fundamental_frequency()

print(f'''Fundamental frequency: {fundamental}''')

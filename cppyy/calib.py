#!/usr/bin/env python3.10

### CPPYY ###

from refspec import SpecConfig, SignalGenerator, SpecOutput, RefSpectrometer

import argparse

#######################################
parser = argparse.ArgumentParser()
parser.add_argument("-v", "--verbose",      action='store_true', help="Verbose mode")

parser.add_argument("-c", "--cal_signal",   action='store_true', help="Cal signal")
parser.add_argument("-p", "--pf_signal",    action='store_true', help="PF signal")
parser.add_argument("-s", "--sky_signal",   action='store_true', help="Sky signal")
# parser.add_argument("-f", "--cachefile",type=str,            help="The optional cache file", default='')
#######################################


args        = parser.parse_args()

verbose     = args.verbose
pf_signal   = args.pf_signal
sky_signal  = args.sky_signal
cal_signal  = args.cal_signal
# -----------------------------------------------------------------------------------------

if verbose: print("*** Verbose mode ***")

# If nothing specified, assume all signals are set:
if (not (pf_signal or sky_signal or cal_signal)): pf_signal=sky_signal=cal_signal=True

if verbose: print(f'''PF Signal: {pf_signal}, Sky Signal: {sky_signal}, Cal Signal: {cal_signal}''')


cfg = SpecConfig()


cfg.Nfft            = 4096
cfg.Ntaps           = 6
cfg.Nchannels       = 1
cfg.Average1Size    = 50
cfg.Average2Size    = 20
cfg.notch           = False
cfg.Ncalib          = 1024
cfg.calib_odd       = True

pk_fname            = "data/Pk/Pk_wnoise.txt"
cal_fname           = "data/samples/calib_filt.txt"
out_root            = ""

fundamental  = cfg.fundamental_frequency()

block_size   = cfg.Nfft

exit(0)

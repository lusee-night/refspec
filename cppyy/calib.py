#!/usr/bin/env python3.10

### CPPYY ###

from refspec import SpecConfig, SignalGenerator, SpecOutput, RefSpectrometer

import argparse

#######################################
parser = argparse.ArgumentParser()
parser.add_argument("-v", "--verbose",      help="Verbose mode",    action='store_true')

parser.add_argument("-c", "--cal_signal",   help="Cal signal",      action='store_true')
parser.add_argument("-p", "--pf_signal",    help="PF signal",       action='store_true')
parser.add_argument("-s", "--sky_signal",   help="Sky signal",      action='store_true')

parser.add_argument("-n", "--notch",        help="Spectral notch",  action='store_true')
parser.add_argument("-A", "--amplitude_rescaling",
                                            help="Calibration Amplitude Rescaling", type=float, default=1.0)

parser.add_argument("-N", "--Ngo",          help="Number of Big Samples",           type=int,   default=120)
parser.add_argument("-a", "--a2",           help="Average 3",                       type=int,   default=20)

parser.add_argument("-d", "--drift",        help="Clock drift in ppm",              type=float, default=0.0)

# parser.add_argument("-f", "--cachefile",type=str,            help="The optional cache file", default='')
#######################################


args        = parser.parse_args()

verbose     = args.verbose

pf_signal   = args.pf_signal
sky_signal  = args.sky_signal
cal_signal  = args.cal_signal

notch       = args.notch
cal_A       = args.amplitude_rescaling

Ngo         = args.Ngo
a2          = args.a2
drift       = args.drift
# -----------------------------------------------------------------------------------------

if verbose: print("*** Verbose mode ***")

# If nothing specified, assume all signals are set:
if (not (pf_signal or sky_signal or cal_signal)):
    pf_signal=sky_signal=cal_signal=True
    if verbose: print('*** No signals specified, assuming all ***')

if verbose:
    print(f'''*** PF Signal: {pf_signal}, Sky Signal: {sky_signal}, Cal Signal: {cal_signal}, Spectral Notch: {notch}, Amplitude Rescaling: {cal_A} ***''')
    print(f'''*** Number of big samples: {Ngo}, Average 2: {a2}, Drift (ppm): {drift} ***''')


cfg = SpecConfig()


cfg.Nfft            = 4096
cfg.Ntaps           = 6
cfg.Nchannels       = 1
cfg.Average1Size    = 50
cfg.Average2Size    = a2
cfg.notch           = notch
cfg.Ncalib          = 1024
cfg.calib_odd       = True

pk_fname            = "data/Pk/Pk_wnoise.txt"
cal_fname           = "data/samples/calib_filt.txt"
out_root            = ""

fundamental  = cfg.fundamental_frequency()

block_size   = cfg.Nfft

exit(0)

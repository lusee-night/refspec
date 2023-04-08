#!/usr/bin/env python3.10

### CPPYY ###

from refspec import SpecConfig, PowerSpecSource, CombSource, SignalCombiner, SpecOutput, RefSpectrometer

from cppyy.gbl.std import vector

import argparse

#######################################
parser = argparse.ArgumentParser()
parser.add_argument("-v", "--verbose",      help="Verbose mode",                    action='store_true')

parser.add_argument("-c", "--cal_signal",   help="Cal signal",                      action='store_true')
parser.add_argument("-p", "--pf_signal",    help="PF signal",                       action='store_true')
parser.add_argument("-s", "--sky_signal",   help="Sky signal",                      action='store_true')
parser.add_argument("-n", "--notch",        help="Spectral notch",                  action='store_true')

parser.add_argument("-A", "--ampl_resc",    help="Calibration Amplitude Rescaling", type=float, default=1.0)

parser.add_argument("-N", "--Ngo",          help="Number of Big Samples",           type=int,   default=120)
parser.add_argument("-a", "--a2",           help="Average 2",                       type=int,   default=20)

parser.add_argument("-d", "--drift",        help="Clock drift in ppm",              type=float, default=0.0)

parser.add_argument("-P", "--pk_fname",     help="Filename for power spectrum",     type=str,   default='data/Pk/Pk_wnoise.txt')
parser.add_argument("-C", "--cal_fname",    help="Filename for calibrator",         type=str,   default='data/samples/calib_filt.txt')
parser.add_argument("-F", "--picket_fname", help="Filename for picket fence",       type=str,   default='data/samples/picket_fence_1024.txt')



parser.add_argument("-r", "--shift_rec",    help="No.records to shift calibrator",  type=int,   default=0)

parser.add_argument("-o", "--out_root",     help="Output root",                     type=str,   default='')
parser.add_argument("-S", "--seed",         help="Random seed",                     type=int,   default=100)

#######################################


args        = parser.parse_args()

verbose     = args.verbose

pf_signal   = args.pf_signal
sky_signal  = args.sky_signal
cal_signal  = args.cal_signal

notch       = args.notch
cal_A       = args.ampl_resc

Ngo         = args.Ngo
a2          = args.a2
drift       = args.drift

pk_fname    = args.pk_fname
cal_fname   = args.cal_fname
picket_fname= args.picket_fname

cal_shift   = args.shift_rec
out_root    = args.out_root
seed        = args.seed

# -----------------------------------------------------------------------------------------
if verbose: print("*** Verbose mode ***")

if (not (pf_signal or sky_signal or cal_signal)): # if nothing specified, assume all signals are set
    pf_signal=sky_signal=cal_signal=True
    if verbose: print('*** No signals specified, assuming all ***')

# Parameter printout/check:
if verbose:
    print(f'''*** PF Signal: {pf_signal}, Sky Signal: {sky_signal}, Cal Signal: {cal_signal}, Spectral Notch: {notch}, Amplitude Rescaling: {cal_A} ***''')
    print(f'''*** Number of big samples: {Ngo}, Average 2: {a2}, Drift (ppm): {drift} ***''')
    print(f'''*** Power spectrum filename: {pk_fname}, Calibrator filename: {cal_fname}, Output root: {out_root} ***''')


# -----------------------------------------------------------------------------------------
cfg = SpecConfig() # the configuration object

cfg.Nfft            = 4096
cfg.Ntaps           = 6
cfg.Nchannels       = 1
cfg.Average1Size    = 50
cfg.Average2Size    = a2
cfg.notch           = notch
cfg.Ncalib          = 1024
cfg.calib_odd       = True

cfg.sanity_check()

fundamental         = cfg.fundamental_frequency()
block_size          = cfg.Nfft
Nbins               = cfg.Nbins()

if verbose: print("*** Nbins:", Nbins, "   Ncalib:", cfg.Ncalib, "***")

# Mimic the file names in the C++ source

of      = open(out_root+"powspect.txt", 'w')
ofc     = open(out_root+"calib.txt", 'w')
ofc2    = open(out_root+"calib_meta.txt", 'w')

slist = [] # array of sources

if sky_signal:
    SigNoise = PowerSpecSource(pk_fname, cfg.sampling_rate, block_size, cfg.Nchannels, Ngo*cfg.AverageSize()+cfg.Ntaps, False, False, seed)
    slist.append(SigNoise)
    if verbose: print ("*** Added Sky Signal ***")

if pf_signal:
    PF = CombSource(block_size, cfg.Nchannels, 1024, picket_fname, 1, 0.008)
    slist.append(PF)
    if verbose: print ("*** Added PF Signal ***")

if cal_signal:
    CalSig = CombSource(block_size, cfg.Nchannels, 2048, cal_fname, 16, cal_A, 0.0, drift*1e-6,0.0, cal_shift)
    slist.append(CalSig)
    if verbose: print ("*** Added Cal Signal ***")

# -- Combine all of the optional sources created above:
src             =   SignalCombiner(slist, True)
if verbose: print("*** Signal Combiner Source created ***")

output          =   SpecOutput(cfg)
if verbose: print("*** Output Object created ***")

spectrometer    =   RefSpectrometer(src, cfg)
if verbose: print("*** Spectrometer created ***")


for iNgo in range(Ngo):
    spectrometer.run(output)
    if(verbose): print("*** Source rms: ", src.rms())

    # newlist = [str(x) for x in output.avg_pspec[0]]    
    #for i in range(1, Nbins):
        
        # newlist = [str(x) for x in output.avg_pspec[0]]
        # print(newlist)
        # print(" ".join(newlist))

        # print(output.avg_pspec[0][i])
        # This following line in the C++ source but it's unclear why: k = fundamental*float(i)*(10**(-6))
       


of.close()
ofc.close()
ofc2.close()

exit(0)
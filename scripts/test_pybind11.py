#!/usr/bin/env python3.10


import sys
 
# appending a path
sys.path.append('./')

try:
    import refspec
except:
    print("Error importing main module")

print("refspec docstring:", refspec.__doc__)

classes = {
    'SpecConfig':'Ntaps,Average1Size,Average2Size,AverageSize(),fundamental_frequency(),Ncalib,calib_odd,calib_subint,Nfft,notch',
    'SignalGenerator':''
    }

bag = {}

skip = 'SignalGenerator, SpecOutput'

for item in classes.keys():

    if item in skip: continue

    try:
        bag[item]=eval("refspec."+item+"()")
    except:
        print("Error importing "+item)

    attrs = classes[item].split(",")
    print("***"+item+"***")
    for attr in attrs:
        try:
            print(item, attr, eval("bag['"+item+"']."+attr))
        except:
            print("Error in", item, attr)


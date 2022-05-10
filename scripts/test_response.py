#!/usr/bin/env python3

import argparse
from os import access
import numpy as np

parser = argparse.ArgumentParser()

parser.add_argument("-r", "--reffile",  type=str,               help="Name of the reference file",      required=True)
parser.add_argument("-n", "--newfile",  type=str,               help="Name of the new file",            required=True)
parser.add_argument("-a", "--accuracy", type=float,             help="Accuracy",                        default=0.0001)
parser.add_argument("-v", "--verbose",  action='store_true',    help="Verbose mode")


################################################################
args    = parser.parse_args()

reffile = args.reffile
newfile = args.newfile
verb    = args.verbose
accuracy= args.accuracy


if verb:
    print('*** Verbose mode ***')
    print('*** Testing output of the "test_response" executable ***')

with open(reffile, 'r') as rf:
    rflen = len(rf.readlines())
    if verb: print('Lines in the reference file:', rflen)

with open(newfile, 'r') as nf:
    nflen = len(nf.readlines())
    if verb: print('Lines in new file:', nflen)

if rflen!=nflen:
    if verb: print("Number of lines doesn't match")
    exit(-1)


refdata=np.loadtxt(reffile)[:,1]
newdata=np.loadtxt(newfile)[:,1]

try:
    assert(np.all(np.abs(refdata-newdata)/(0.5*(refdata+newdata))<accuracy))
except:
    exit(-2)

exit(0)

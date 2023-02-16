#!/bin/bash

# NB. Deprected as of Feb 16 2023, as the refspec image has been corrected

wget http://www.fftw.org/fftw-3.3.10.tar.gz
tar xfvz fftw-3.3.10.tar.gz
cd fftw-3.3.10

# Build two versions of the library, with and w/o the float option:

./configure --enable-float --enable-shared
make -j4 # if you have at least 4 cores that helps speed
make install

./configure --enable-shared
make -j4 # if you have at least 4 cores that helps speed
make install

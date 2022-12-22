#!/bin/bash

# NB. This is a provisional script used to test building the Python bindings for refspec,
# used temporarily to not break the Makefile logic while making frequent changes to the code

SRCS="refspec.cpp SpecConfig.cpp SpecOutput.cpp SignalGenerator.cpp RefSpectrometer.cpp PowerSpecSource.cpp pfb.cpp"

g++ -I../include -I../extern -O3 -Wall -shared -std=c++17 -fPIC $(python3 -m pybind11 --includes) ${SRCS} \
-lfftw3 -lfftw3f -o refspec$(python3.10-config --extension-suffix)

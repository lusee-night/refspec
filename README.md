
# refspec - the LuSEE-Night Reference Spectrometer

![example workflow](https://github.com/lusee-night/refspec/actions/workflows/refspec-test.yml/badge.svg)


## Prerequisites 

### FFTW

Installing the FFTW package locally

```bash
wget http://www.fftw.org/fftw-3.3.10.tar.gz
tar xfvz fftw-3.3.10.tar.gz
cd fftw-3.3.10

# Build two versions of the library, with and w/o the float option:

./configure --enable-float
make -j4 # if you have at least 4 cores that helps speed
make install

./configure
make -j4 # if you have at least 4 cores that helps speed
make install
```

### pybind11

The `pybind11` is a header-only library which is one of popular methods
to create Python bindings for C++ code (and one of the most powerful).
There are a ![few ways to install pybind11](https://pybind11.readthedocs.io/en/stable/installing.html)

At the time of writing, we are using the `git submodule` method, and the headers
will go into a subfolder of the `extern` folder. The following two lines were _originally_
used to add `pybind` to this repository:

```bash
git submodule add -b stable ../../pybind/pybind11 extern/pybind11
git submodule update --init
```

Since the `extern` is now in the main branch, it is only necessary for the developer
working on `pybind11` components to issue the second command __after__ the repository is cloned
i.e.

```bash
git submodule update --init
```



## Building for pybind11

An example of a command line (for demonstration only). Note using the automatic generation of relevant
include file names by python3, and automatic naming of the shared library which will be loaded as a
Python module.
```bash
g++ -I../include -I../extern -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) SpecConfig.cpp SpecOutput.cpp SignalGenerator.cpp RefSpectrometer.cpp pfb.cpp -lfftw3 -lfftw3f -o refspec$(python3.10-config --extension-suffix)
```

## CI

The Python interface is now a part of the CI Action sequence on GitHub.

## Docker

A Docker image is in preparation to facilitate testing, see the ```docker```
folder in this repo.

## Checking for memory leaks

When using ```valgrind```, the following option will help eliminate chatty output reporting "uninitialized variables/conditional jump":

```bash
 valgrind --undef-value-errors=no myExecutable.exe
```



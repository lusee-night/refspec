
# refspec - the LuSEE-Night Reference Spectrometer

![example workflow](https://github.com/lusee-night/refspec/actions/workflows/refspec-test.yml/badge.svg)


## Installation of the components

### Installing the FFTW package locally

```bash
wget http://www.fftw.org/fftw-3.3.10.tar.gz
tar xfvz fftw-3.3.10.tar.gz
cd fftw-3.3.10

# Build two versions of the library, with
# and w/o float option enabled:

./configure --enable-float
make -j4 # if you have at least 4 cores that helps speed
make install

./configure
make -j4 # if you have at least 4 cores that helps speed
make install
```

## pybind11

The `bybind11` is a header-only library which is one of popular methods
to create Python bindings for C++ code (and one of the most powerful).
There are a ![few ways to install pybind11](https://pybind11.readthedocs.io/en/stable/installing.html)

At the time of writing, we are using the `git submodule` method, and the headers
will go into a subfolder of the `extern` folder. The following two lines were used
to add `pybind` to this repository:

```bash
git submodule add -b stable ../../pybind/pybind11 extern/pybind11
git submodule update --init
```

## Docker

A Docker image is in preparation to facilitate testing, see the ```docker```
folder in this repo.

## Checking for memory leaks

When using ```valgrind```, the following option will help eliminate chatty output reporting "uninitialized variables/conditional jump":

```bash
 valgrind --undef-value-errors=no myExecutable.exe
```




# refspec - the LuSEE-Night Reference Spectrometer

![example workflow](https://github.com/lusee-night/refspec/actions/workflows/refspec-test.yml/badge.svg)

## Dependency: the FFTW package

Installing the FFTW package locally

```bash
wget http://www.fftw.org/fftw-3.3.10.tar.gz
tar xfvz fftw-3.3.10.tar.gz
cd fftw-3.3.10

# IMPORTANT, in the current version: 
# Build two versions of the library, with and w/o the float option enabled.
#
# If you have a few cores you can use the "j" option, it really
# helps speed; in the example below the core count is set to 4

./configure --enable-float
make -j4
make install

./configure
make -j4
make install
```

## Docker

A Docker image is in preparation to facilitate testing, see the ```docker```
folder in this repo.

## Checking for memory leaks

When using ```valgrind```, the following option will help eliminate chatty output reporting "uninitialized variables/conditional jump":

```bash
 valgrind --undef-value-errors=no myExecutable.exe
 # And to actually check for leaks:
 valgrind --undef-value-errors=no --leak-check=yes myExecutable.exe
```



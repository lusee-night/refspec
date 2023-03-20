
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

./configure --enable-float --enable-shared
make -j4 # if you have at least 4 cores that helps speed
make install

./configure --enable-shared
make -j4 # if you have at least 4 cores that helps speed
make install
```

The `--enable-shared` option is important, as otherwise the shared version of the libraries won't be created
and the software won't run properly. The script `fft.sh` has the same functionality and is kept in
the "attic" folder for reference, it's no longer used in the build.


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



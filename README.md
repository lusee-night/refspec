
# refspec - the LuSEE-Night Reference Spectrometer

![example workflow](https://github.com/lusee-night/refspec/actions/workflows/refspec-test.yml/badge.svg)

## Installing the FFTW package locally

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

## Docker

A Docker image is in preparation to facilitate testing, see the ```docker```
folder in this repo.



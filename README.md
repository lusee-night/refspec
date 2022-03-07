# refspec -- the LuSEE-Night Reference Spectrometer


## Installing the FFTW package

```bash
wget http://www.fftw.org/fftw-3.3.10.tar.gz
tar xfvz fftw-3.3.10.tar.gz
cd fftw-3.3.10

# Build two versions of the library, with
# and w/o fload option enabled:

./configure --enable-float
make -j4 # if you have at least 4 cores that helps speed
make install

./configure
make -j4 # if you have at least 4 cores that helps speed
make install
```



# Source files for refspec


## Generic build
```bash
g++ -I../include -shared -Wl,-soname,libspecconfig.so -o libspecconfig.so SpecConfig.cpp
```

## Building for pybind11


### Integrated module

```bash
g++ -I../include -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) refspec.cpp  SpecConfig.cpp SpecOutput.cpp SignalGenerator.cpp -lfftw3 -lfftw3f -o refspec$(python3.10-config --extension-suffix)
```
The FFT libraries are needed for the generator code, if not included in a specific test these can be dropped.


### Building separate modules for each class (deprecated

```bash
g++ -I../include -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) SpecConfig.cpp -o SpecConfig$(python3.10-config --extension-suffix)
g++ -I../include -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) SpecOutput.cpp -o SpecOutput$(python3.10-config --extension-suffix)
```

## Simple Testing

```bash
>>> import refspec
>>> from refspec import SpecConfig
>>> x=SpecConfig()
>>> x.get_Ntaps()
8
>>> x.Nbins()
2049
>>> x.fundamental_frequency()
24414.0625
>>> from refspec import SpecOutput
>>> y=SpecOutput(x)
>>> y.get_Nfft()
4096
>>>
```




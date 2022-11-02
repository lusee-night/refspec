# Source files for refspec


## Generic build
```bash
g++ -I../include -shared -Wl,-soname,libspecconfig.so -o libspecconfig.so SpecConfig.cpp
```

## Building for pybind11

Building separate modules for each class:

```bash
g++ -I../include -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) SpecConfig.cpp -o SpecConfig$(python3.10-config --extension-suffix)
g++ -I../include -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) SpecOutput.cpp -o SpecOutput$(python3.10-config --extension-suffix)
```

Integrated module:

```bash
g++ -I../include -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) refspec.cpp SpecConfig.cpp SpecOutput.cpp -o refspec$(python3.10-config --extension-suffix)
```

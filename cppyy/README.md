# Refspec bindings based on cppyy


## Python bindings with cppyy

There are multiple ways to implement Python bindings for C++ code. One option previously investigated
was the `pybind11` C++ macro library, which allows to build native bindings at compile time. This,
however, required a substantial amount of extra code. The choice finally made was the `cppyy`
Python package, which leverages the `cling` interpreter, making it possible to do some
of the work dynamically at runtime (e.g. automatically parse the C++ header files),
drastically reducing the amount of extra code necessary for the implementation of the bindings.

The `cppyy` Python package can be installed using `pip`.
Conventional shared libraries and header files can be used with `cppyy`, so no custom
build is required. The location of libraries and header files need to be available at
run time, which can be accomplished using the standard environment variables `CPATH`
and `LD_LIBRARY_PATH`.

The `cppyy` package provides a fairly transparent interface to STL objects,
for example a common Python array of objects of type `T` will be translated into a
`std::vector<T>` on the C++ side, automatically -- with rare caveats.

As can be expected, instantiattion of arrays of objects of abstract classes is not
going to work, so care must be taken to implement all pure virtual functions before
attempting to put Python bindings in place.



## Appendix

### pybind11 (for historical reference only)

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

To build a pybind11-compiant shared library, one must specify the correct location of the header files
and also properly name the shared library -- which process can be automates. Below is
an example of a command line (for demonstration only). Note using the automatic generation of relevant
include file names by python3, and automatic naming of the shared library which will be loaded as a
Python module.
```bash
g++ -I../include -I../extern -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) SpecConfig.cpp SpecOutput.cpp SignalGenerator.cpp RefSpectrometer.cpp pfb.cpp -lfftw3 -lfftw3f -o refspec$(python3.10-config --extension-suffix)
```

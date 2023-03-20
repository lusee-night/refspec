# Reference Spectrometer

## Docker

At the time of writing, two images are maintained in the official [LuSEE repository on Docker Hub](https://hub.docker.com/repositories/lusee).
* [The "foundation image"](https://hub.docker.com/repository/docker/lusee/lusee-night-refspec-foundation/general

## Prerequisites for Python bindings

### About

There are multiple ways to implement Python bindings for C++ code. Two options are currently
being investigated:

* pybind11
* cppyy

The former framework relies on advanced C++ macros to create a shared library that can be loaded
into a Python application as a module. It works well but requires a non-trivial amount of C++
boilerplate code, especially to convert C++ data structures into common Python objects e.g.
`numpy` arrays.

The latter utilizes the `clang` interpreter to inspect the C++ header files and thus be able
to parse a conventional shared library from within a Python application, so effectively
is an inversion of the process used in pybind11. 

Some technical details on both are given below.

### pybind11

The `pybind11` is a header-only library which is one of popular methods
to create Python bindings for C++ code (and one of the most powerful).
There are a [few ways to install pybind11](https://pybind11.readthedocs.io/en/stable/installing.html)

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

### cppyy

This Python package can be installed using `pip`.
Conventional shared libraries and header files can be used with `cppyy`, so no custom
build is required. The location of libraries and header files need to be available at
run time, which can be accomplished using the standard environment variables `CPATH`
and `LD_LIBRARY_PATH`.

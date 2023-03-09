# Python scripts

## Python binding of refspec C++ classes

The environment variables `CPATH` and `LD_LIBRARY_PATH` need to be set
to point to the location (preferably absolute path) of the header files
and the refspect.so respectively.

Two binding methods have been partially implemented and tested:
* pybind11
* cppyy

For the cppyy version please see the dedicated folder `cppyy`.

## Suite of CI scripts

```bash
usage: test_response.py [-h] -r REFFILE -n NEWFILE [-a ACCURACY] [-v]

options:
  -h, --help            show this help message and exit
  -r REFFILE, --reffile REFFILE
                        Name of the reference file
  -n NEWFILE, --newfile NEWFILE
                        Name of the new file
  -a ACCURACY, --accuracy ACCURACY
                        Accuracy
  -v, --verbose         Verbose mode
```

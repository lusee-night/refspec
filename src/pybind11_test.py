#!/usr/bin/env python3
# A simple demo of the pybind11 approach to python binding


import SpecConfig
import SpecOutput

from SpecConfig import SpecConfig
from SpecOutput import SpecOutput

sc=SpecConfig()
so=SpecOutput(sc)

print(so.get_Nfft())
print(so.get_mode())

exit(0)


#!/usr/bin/env python3


import ctypes

lib                     = ctypes.cdll.LoadLibrary('./libspecconfig.so')
# lib.sx_new.argtypes     = [ctypes.c_void_p]
#lib.sc_Nfft.argtypes    = [ctypes.c_void_p]
#lib.sc_Nfft.restype     = ctypes.c_ulong

#sc  = lib.sc_new()
#x   = lib.sc_Nfft(sc)
#print(x)


class SpecConfig(object):
    def __init__(self):

        lib.sc_Nfft.argtypes            = [ctypes.c_void_p]
        lib.sc_Nfft.restype             = ctypes.c_ulong

        lib.sc_sampling_rate.argtypes   = [ctypes.c_void_p]
        lib.sc_sampling_rate.restype    = ctypes.c_double

        self.obj = lib.sc_new()

    def Nfft(self):
        return lib.sc_Nfft(self.obj)
    
    def sampling_rate(self):
        return lib.sc_sampling_rate(self.obj)
    
# -------------------------------

sc = SpecConfig()

print(sc.Nfft())
print(sc.sampling_rate())

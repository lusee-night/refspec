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

        lib.sc_Nfft.argtypes    = [ctypes.c_void_p]
        lib.sc_Nfft.restype     = ctypes.c_ulong

        self.obj = lib.sc_new()

    def Nfft(self):
        return lib.sc_Nfft(self.obj)
    


sc = SpecConfig()
print(sc.Nfft())
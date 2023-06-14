import cppyy
from cppyy import ll
import numpy as np
import scipy.linalg as la

headers = [
    'CombSource.h',
    'FileStreamSource.h',
    'FileStreamSink.h',
    'pfb.h',
    'PowerSpecSource.h',
    'CorrelatedSpecSource.h',
    'RefSpectrometer.h',
    'SignalCombiner.h',
    'SignalGenerator.h',
    'SignalSource.h',
    'SpecConfig.h',
    'SpecOutput.h',
    'WhiteNoise.h',
    'testing.h'
]

for header in headers:
    try:
        print ("Loading ",header)
        cppyy.include("/home/anze/work/lusee/refspec/include/"+header)
    except:
        print(f'''Error loading {header}, exiting''')
        exit(-1)

cppyy.load_library('refspec.so')

from cppyy.gbl import RefSpectrometer, PowerSpecSource, CombSource, SignalGenerator, \
                      SignalCombiner, SignalSource, SpecOutput, FileStreamSource, FileStreamSink, WhiteNoise
            



    
def CorrelatedSpecSource(f, Pmat, sampling_rate, block_size, Nchannels, Nblocks, 
                 repeat=False, second_fourier=False, seed=123, verbose=False):

    assert(Pmat.shape==(len(f),Nchannels,Nchannels))
    obj = cppyy.gbl.CorrelatedSpecSource(f,sampling_rate, block_size, Nchannels, Nblocks, repeat,
                        second_fourier, seed, verbose)
    Pchol = np.array([la.cholesky(C) for C in Pmat])
    for i,f_ in enumerate(f):
        for j in range(Nchannels):
            for k in range(Nchannels):
                obj._set_internal_C(i,j,k,Pchol[i,j,k])
    obj._go_internal(f, seed)
    return obj

class SpecConfig(cppyy.gbl.SpecConfig):
    
    def set_zoom_weights (self,arr):
        N,M = arr.shape
        self.resize_zoom(N,M)
        for i in range(N):
            for j in range(M):
                self.set_zoom_weight(i,j,np.real(arr[i,j]),np.imag(arr[i,j]))

    def set_zoom (self,Nchan, Ntaps,window=None, antialias=True):
        def onzero(i):
            a=np.zeros(Nchan,complex)
            a[i]=1.0
            return a
        mat0 = np.array([np.fft.ifft(onzero(i),Nchan) for i in range(Nchan//2,-Nchan//2,-1)])
        mat = np.hstack([mat0]*Ntaps)
        L = Nchan*Ntaps
        if Ntaps>1:
            x=np.pi*(np.arange(L)-L/2+0.5)/Nchan
            pfb = np.sin(x)/(x+1e-30)     
            pfb[x==0.0]=1.0    
        else:
            pfb = np.ones(L)
        
        if window is not None:
            pfb *= window(len(pfb))
        mat = mat*pfb[None,:]    

        if Nchan%2==1 and antialias:
            x = np.fft.fft(mat[0,:])
            x[len(x)//2-Ntaps//2+1:]=0.0
            mat[0,:] = np.fft.ifft(x)
            mat[-1,:] = np.conj(mat[0,:])
            
        self.set_zoom_weights(mat)

    def __del__(self):
        print ("SpecConfig deleted")


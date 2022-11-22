import numpy as np
import os
try:
    import matplotlib.pyplot as plt
except:
    pass
import pickle
import hashlib
from functools import cache

def shift_vec(v,i):
    if i==0:
        return v
    else:
        return np.hstack((v[i:],v[:i]))
    


class CalRun:
    def __init__(self, A=1.0, sky=True, pf=True, cal=True, notch=True, N=120, A2=20, drift = 0, 
                 pkfile = "data/Pk/Pk_wnoise.txt", calfile = "data/samples/calib_filt.txt", 
                 cal_shift = 0, seed=123, verbose=False, force=False):
        drift= float(drift)
        
        self.N=N
        self.A=A
        self.input_drift = drift
        opt = f"-A {A} -N {N} --a2 {A2} -d {drift} --pkfn {pkfile} --calfn {calfile} --calshift {cal_shift} --seed {seed} "
        if sky: opt+=" -s"
        if pf: opt+=" -p"
        if cal: opt+=" -c"
        if notch: opt+=" -n"
        runstr = "./calib.exe "+opt
        self.hash = hashlib.md5(runstr.encode("utf-8")).hexdigest()
        cfname = f"cache/{self.hash}.pickle"
        print (cfname)
        if (not force) and os.path.isfile(cfname):
            try:
                loaded=pickle.load(open(cfname,"rb"))
                self.__dict__.update(loaded.__dict__)
                return
            except:
                print ("Ignoring broken file.")

        runstr += f" --outroot {self.hash}"
        if verbose: 
            print ("Running: ",runstr)
        else:
            runstr += " >/dev/null"
        os.system(runstr)
        self.Pk = np.loadtxt(self.hash+'powspec.txt')
        self.cal = np.loadtxt(self.hash+'calib.txt')
        self.cal = np.atleast_2d(self.cal)
        self.idrift, self.cal_snr = np.loadtxt(self.hash+'calib_meta.txt').T  #internal drift
        self.idrift = np.atleast_1d(self.idrift)
        os.remove(self.hash+'powspec.txt')
        os.remove(self.hash+'calib.txt')
        os.remove(self.hash+'calib_meta.txt')
        self.idrift[self.idrift>4] = -(9-self.idrift[self.idrift>4])
        self.idrift= -self.idrift.astype(int)
        self.k = np.arange(1,2049)*0.025
        self.get_cal()
        pickle.dump(self,open(cfname,"wb"))
            

        
    def get_cal_old(self):
        acal = np.copy(self.cal)
        driftl = []
        
        while len(acal)>1:
            cdrift = []
            N = len (acal)
            nacal = []
            for i in range(0,N,2):
                if (i+1==N):
                    nacal.append(self.cal[i]) ## last one, nothing to add
                else:
                    v1 = acal[i]
                    v2 = acal[i+1] 
                    v20 = v2+v1
                    v2m = v2+np.hstack((v1[1:],v1[0]))     
                    v2p = v2+np.hstack((v1[-1],v1[:-1]))  ## follow convention
                    var0 = np.dot(v20,v20)
                    varp = np.dot(v2p,v2p)
                    varm = np.dot(v2m,v2m)
                    vars = np.array([var0,varp,varm])
                    
                    det = vars.max()/vars.min()
                    ivars = vars.argmax()
                    nacal.append([v20,v2p,v2m][ivars])
                    if ivars==2: ivars = -1
                    cdrift.append(ivars)
            acal = nacal
            driftl.append(cdrift)
        ## now reverse add cdrift
        drift = driftl.pop()
        while len(driftl)>0:
            line = driftl.pop()
            # instert the last one at odd places
            outline = np.zeros(len(drift)+len(line))
            outline[::2] = np.array(line)
            outline[1::2] = drift
            drift = outline

        self.drift = drift
        cumdrift = []
        t=0
        for d1,d2 in zip(self.idrift, self.drift):
            for j in range(4):
                t+=d1/4
                cumdrift.append(t)
            t+=d2
            cumdrift.append(t)
        for j in range(4):
            t+=self.idrift[-1]/4
            cumdrift.append(t)
        cumdrift.append(t)
        self.cumdrift=cumdrift
        self.drift_ppm = self.cumdrift[-1]/2/(self.N*50*20*4096)/1e-6
        
    def get_cal_old2(self):
        maxvar=0
        cumidrifts = np.cumsum(self.idrift[::-1]).astype(int)[::-1]
        print (len(cumidrifts), cumidrifts)
        for trial in np.linspace(-self.N+1, self.N-1,self.N*2-1):
            
            ndx = -cumidrifts
            ndx += np.floor(np.linspace(trial,0,self.N)+0.5).astype(int)
            #ndx = 
            print (trial, ndx)
            ctry = np.array([shift_vec(self.cal[k],i) for k,i in enumerate(ndx)]).sum(axis=0)
            #print (ctry)
            var=(ctry**2).sum()
            #print(ndx[0]/(self.N*50*20*4096)/1e-6, var, maxvar)
            if var>maxvar:
                maxvar=var
                drift=trial
                best_vec = ctry
                best_ndx = ndx
        
        self.cumdrift=best_ndx
        self.ldrift = drift
        self.cal_vec = best_vec
        self.drift_ppm = (self.cumdrift[0])/((self.N*5-1)*800*1024)/1e-6

    def get_cal(self):
        maxvar=0
        addshift = []
        vb = None
        #def add4 (ls,w):
        #    for i in range(4):
        #        ls.append(ls[-1]+w/4)
        def add4 (ls,i):
            s = ls[-1]
            if i==4:
                ls+=[s+1,s+2,s+3,s+4]
            elif i==3:
                ls+=[s+1,s+2,s+2,s+3]
            elif i==2:
                ls+=[s+1,s+1,s+2,s+2]
            elif i==1:
                ls+=[s+0,s+0,s+1,s+1]
            elif i==0:
                ls+=[s+0,s+0,s+0,s+0]
            elif i==-1:
                ls+=[s-0,s-0,s-1,s-1]
            elif i==-2:
                ls+=[s-1,s-1,s-2,s-2]
            elif i==-3:
                ls+=[s-1,s-2,s-2,s-3]
            elif i==-4:
                ls+=[s-1,s-2,s-3,s-4]
     
                
        for cal,idrift in zip(self.cal[::-1], self.idrift[::-1]):
            if vb is None:
                vb = np.copy(cal) #shift_vec(cal, +idrift)
                cdrift = idrift
                fdrift = [0]
                add4(fdrift, idrift)
            else:
                v2=shift_vec(cal,cdrift)
                v2p=shift_vec(cal,cdrift+1)
                v2m=shift_vec(cal,cdrift-1)
                s0 = np.dot(vb,v2)
                sp = np.dot(vb,v2p)
                sm = np.dot(vb,v2m)
                #print (s0,sp,sm)
                #zero = np.random.uniform(0,1)>0.53
    
                if (s0>sp) and (s0>sm):# and True:# and False: 
                    addshift=0
                    vb+=v2
                elif (sp>s0) and(sp>sm):
                    addshift=+1
                    vb+=v2p
                else:
                    addshift=-1
                    vb+=v2m
                fdrift.append(fdrift[-1]+addshift)
                add4(fdrift,idrift)
                cdrift += +idrift
                cdrift+=addshift
                
        #print('var=',vb.var()/1e5)      
        self.cal_vec = vb
        self.cumdrift = np.array(fdrift[::-1])
        self.drift_ppm = (self.cumdrift[0])/((self.N*5-1)*800*1024)/1e-6
        
        
        
    def get_sig(self,highsnr):
        mvecf = np.fft.rfft(np.hstack((self.cal_vec, -self.cal_vec)))[1::2]
        tvecfc= np.conj(np.fft.rfft(np.hstack((highsnr.cal_vec, -highsnr.cal_vec))))[1::2]
        Ntr = 2048*16
        tphase=np.arange(Ntr)/Ntr*2*np.pi
        ndx = np.arange(1025)[1::2] #2048/2
        #rint (len(ndx), len(mvecf))
        
        #xcor = np.array([np.sum(np.imag(mvecf*np.exp(1j*ndx*phase)*tvecfc)) for phase in tphase])
        ##plt.plot(xcor)
        self.tpwr = np.abs(tvecfc**2)
        self.mpwr = np.abs(mvecf**2)
        xcor = np.array([np.sum(np.real(mvecf*np.exp(1j*ndx*phase)*tvecfc/self.tpwr)) for phase in tphase])
        #plt.plot(xcor)
        #for i in [0]:#range(-5,+5):
        self.cal_shift = xcor.argmax()
        phase = tphase[xcor.argmax()]
        #print(xcor.argmax(),len(xcor))
        self.tcor = (mvecf*np.exp(1j*ndx*phase)*tvecfc)
        self.rbeam = self.tcor/self.tpwr
        self.cf = ndx*0.05
        #plt.plot(self.cf,self.rbeam)
        
                            
    def get_sig2(self,offset, drift, template):
        mvecf = np.fft.rfft(np.hstack((self.cal_vec, -self.cal_vec)))[1::2]
        tvec = CalPredictor(offset,drift,self.cumdrift.tobytes(),template)
        self.tvec=tvec
        tvecfc= np.conj(np.fft.rfft(np.hstack((tvec, -tvec))))[1::2]
        Ntr = 2048*16
        tphase=np.arange(Ntr)/Ntr*2*np.pi
        ndx = np.arange(1025)[1::2] #2048/2
        self.tpwr = np.abs(tvecfc**2)
        self.mpwr = np.abs(mvecf**2)
        xcor = np.array([np.sum(np.real(mvecf*np.exp(1j*ndx*phase)*tvecfc/self.tpwr)) for phase in tphase])
        print ("max cor:", xcor.max())
        self.cal_shift = xcor.argmax()
        phase = tphase[xcor.argmax()]
        self.tcor = (mvecf*np.exp(1j*ndx*phase)*tvecfc)
        self.rbeam = self.tcor/self.tpwr
        self.cf = ndx*0.05
        
@cache
def CalPredictor(offset, drift, cumdrift, template):
    if type(cumdrift)==bytes:
        cumdrift = np.frombuffer(cumdrift,int)
    ddrift = cumdrift[:-1]-cumdrift[1:]
    #print (cumdrift[0])
    #ddrift*=-1
    pat =""
    for x in ddrift:
        if x==+1:
            pat+="+"
        elif x==-1:
            pat+="-"
        elif x==0:
            pat+="0"
    assert(len(ddrift)==len(pat))

    torun = f"./drift_pred.exe {template} {offset} {drift} {pat}"
    #print (torun)
    os.system(torun)
    vec = np.loadtxt("driftpred.txt")
    os.remove("driftpred.txt")
    return vec
              
              
            
              
#def CalPredictor(offset, drift, cumdrift, template):
#    template = np.loadtxt(template)
#    assert(len(template)==16*2048)
#    alpha = +drift*1e-6
#    N1=400
#    Nb = len(cumdrift)
#    vec=np.zeros(2048)
#    cumdrift-=cumdrift[-1]
#    for i,d in enumerate(cumdrift):
#        t0=2048+(alpha)*(N1*2048)*i
#        t = t0+d-offset/16+np.arange(N1*2048)*(1+alpha)
#        ndx = ((t*16).astype(int))%(2048*16)
#        toadd=template[ndx].reshape((N1,2048)).sum(axis=0)
#        vec+=toadd
#    return vec*3000

    
    
    
                
                        
                    
                    
              
                
    
        
    
        
        
        

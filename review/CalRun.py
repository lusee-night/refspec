import numpy as np
import os
import matplotlib.pyplot as plt
import pickle
import hashlib

def shift_vec(v,i):
    if i==0:
        return v
    else:
        return np.hstack((v[i:],v[:i]))
    


class CalRun:
    def __init__(self, A=1.0, sky=True, pf=True, cal=True, notch=True, N=120, drift = 0, 
                 pkfile = "data/Pk/Pk_wnoise.txt", calfile = "data/samples/calib_filt.txt", 
                 cal_shift = 0, verbose=False
                ):
        drift= float(drift)
        
        self.N=N
        self.A=A
        self.input_drift = drift
        opt = f"-A {A} -N {N} -d {drift} --pkfn {pkfile} --calfn {calfile} --calshift {cal_shift}"
        if sky: opt+=" -s"
        if pf: opt+=" -p"
        if cal: opt+=" -c"
        if notch: opt+=" -n"
        runstr = "rm powspec.txt calib.txt calib_meta.txt; ./calib.exe "+opt
        self.hash = hashlib.md5(runstr.encode("utf-8")).hexdigest()
        cfname = f"cache/{self.hash}.pickle"
        #print (cfname)
        if os.path.isfile(cfname):
            loaded=pickle.load(open(cfname,"rb"))
            self.__dict__.update(loaded.__dict__)
            return

        if verbose: 
            print ("Running: ",runstr)
        else:
            runstr += " >/dev/null"
        os.system(runstr)
        self.Pk = np.loadtxt('powspec.txt')
        self.cal = np.loadtxt('calib.txt')
        self.idrift, self.cal_snr = np.loadtxt('calib_meta.txt').T  #internal drift
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
        def add4 (ls,w):
            for i in range(4):
                ls.append(ls[-1]+w/4)
                
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
        self.drift_ppm = (self.cumdrift[0])/((self.N*5)*800*1024)/1e-6
        
        
        
    def get_sig(self,highsnr):
        mvecf = np.fft.rfft(np.hstack((self.cal_vec, -self.cal_vec)))[1::2]
        tvecfc= np.conj(np.fft.rfft(np.hstack((highsnr.cal_vec, -highsnr.cal_vec))))[1::2]
        Ntr = 2048*16
        tphase=np.arange(Ntr)/Ntr*2*np.pi
        ndx = np.arange(1025)[1::2] #2048/2
        print (len(ndx), len(mvecf))
        
        #xcor = np.array([np.sum(np.imag(mvecf*np.exp(1j*ndx*phase)*tvecfc)) for phase in tphase])
        ##plt.plot(xcor)
        self.tpwr = np.abs(tvecfc**2)
        self.mpwr = np.abs(mvecf**2)
        xcor = np.array([np.sum(np.real(mvecf*np.exp(1j*ndx*phase)*tvecfc/self.tpwr)) for phase in tphase])
        #plt.plot(xcor)
        #for i in [0]:#range(-5,+5):
        self.calshift = xcor.argmax()
        phase = tphase[xcor.argmax()]
        print(xcor.argmax(),len(xcor))
        self.tcor = (mvecf*np.exp(1j*ndx*phase)*tvecfc)
        self.rbeam = self.tcor/self.tpwr
        self.cf = ndx*0.05
        #plt.plot(self.cf,self.rbeam)
        
                            
            
        
            
                
                        
                    
                    
              
                
    
        
    
        
        
        
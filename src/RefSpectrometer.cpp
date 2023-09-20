#include "RefSpectrometer.h"
#include "assert.h"
#include <iostream>
#include <math.h>

RefSpectrometer::RefSpectrometer (SignalSource *source, SpecConfig const *config):
  source(source), c(config), pfb(c->sampling_rate, c->Nfft, c->Ntaps, c->window)
{
  buffer = new float**[c->Nchannels];

  // Now, we will allocate and copy/difference data only if required
  for (size_t i=0;i<c->Nchannels;i++) {
    if (c->minus_channel[i]>=0) {
      assert(false); // not implemented
      //for (size_t j=0;j<c->Ntaps;j++) data_buffer[iS][j] = fftwf_alloc_real(c->Nfft);
    } else {
      assert (c->plus_channel[i]>=0);
    }
  }

  for (size_t i=0;i<c->Nchannels;i++) buffer[i] = new float*[c->Ntaps];


  pfb_out = new fftwf_complex**[c->Average1Size];
  for (size_t i=0;i<c->Average1Size;i++) {
    pfb_out[i] = new fftwf_complex*[c->Nchannels];
    for (size_t j=0;j<c->Nchannels;j++) pfb_out[i][j]=fftwf_alloc_complex(2*pfb.get_Ncomplex());
  }
  pfb.setup_plan(pfb_out[0][0]);

  // prefill buffer
  for (size_t i=1;i<c->Ntaps;i++) {
    float* cdata[MAX_CHANNELS];
    assert (source->data_available()); // at the beginning we should really have some data
    source->next_block(cdata);
    for (size_t j=0;j<c->Nchannels;j++) buffer[j][i]=cdata[j];
  }
  counter = 0;

  if (c->Ncalib>0) {
    cal_Nsubopt = 2*c->calib_subint-1;
    cal_Ntot = c->AverageSize()*c->Nfft / c->Ncalib;
    cal_Nsub = cal_Ntot / c->calib_subint;
    calbuf = Vec3(cal_Nsubopt,Vec2(c->Nchannels, Vec1(c->Ncalib,0.0)));
    //std::cout<<calbuf.size() <<" " <<calbuf[3].size() << " " << calbuf[3][0].size() <<std::endl;
  }

}


void RefSpectrometer::print_info() {
  std::cout << " Sampling rate   : " << c->sampling_rate <<std::endl;
  std::cout << " FFT Size        : " << c->Nfft <<std::endl;
  std::cout << " Nchannels       : " << c->Nchannels <<std::endl;
  std::cout << " Bin spacing     : " << c->fundamental_frequency()/1e3 <<"kHz" <<std::endl;
  std::cout << " Average 1       : " << c->Average1Size << " ~ " << c->Average1Size*c->Nfft/c->sampling_rate*1e3 <<"ms"<<  " ~ " << (1/(c->Average1Size*c->Nfft/c->sampling_rate*1e3))<<"kHz"<<   std::endl;
  std::cout << " Average 2       : " << c->Average2Size << std::endl;
  std::cout << " Ncalib          : " << c->Ncalib <<std::endl;
  if (c->Ncalib>0) {
    std::cout << " Calib Average1  : " << cal_Nsub << " ~ " << cal_Nsub*c->Ncalib/c->sampling_rate*1e3<<"ms"<<std::endl;
    std::cout << " Calib Average2  : " << cal_Ntot/cal_Nsub << std::endl;
  }
  std::cout << " Total Average   : " << c->AverageSize() << " ~ "  << c->AverageSize()*c->Nfft/c->sampling_rate*1e3 <<"ms"<<    std::endl;
}


void RefSpectrometer::run_pfb(float**cdata, SpecOutput *res, SpecOutput *avg1) {

    // shift buffers by one unit
    for (size_t j=0;j<c->Nchannels;j++) {
      for (size_t i=0;i<c->Ntaps-1;i++) {
	buffer[j][i] = buffer[j][i+1];
      }
    }
    for (size_t j=0;j<c->Nchannels;j++) {
      buffer[j][c->Ntaps-1] = cdata[j];
      pfb.process(buffer[j], pfb_out[pfb_counter][j]);
    }

    pfb_counter++;
    if (pfb_counter == c->Average1Size) {
      process_output(avg1);
      *res += *avg1;
      pfb_counter = 0;
    }
}

int router5 (size_t sub, size_t j) {

  const int res1[9] =  { 0, 0, 1, 1, 1, -1, -1, -1,  0 };
  const int res2[9] =  { 0, 0, 1, 2, 2, -2, -2, -1,  0 };
  const int res3[9] =  { 0, 1, 2, 2, 3, -3, -2, -2, -1 };
  const int res4[9] =  { 0, 1, 2, 3, 4, -4, -3, -2, -1 };
  assert ((sub>=0)&(sub<5));
  switch (sub) {
  case 4:
    return 0;
  case 3:
    return res1[j];
  case 2:
    return res2[j];
  case 1:
    return res3[j];
  case 0:
    return res4[j];
  }
  return 0; // never actually executed
}

void RefSpectrometer::zero_calbuf() {
    for (size_t i=0;i<cal_Nsubopt;i++) {
      for (size_t j=0;j<c->Nchannels;j++) {
	for (size_t k=0;k<c->Ncalib;k++) {
	  calbuf[i][j][k]=0.0;
	}
      }
    }
}

void RefSpectrometer::run_calib(float**cdata, SpecOutput *res) {
  size_t  csub = cal_c / cal_Nsub;
  bool done = false;
  float w;

  for (size_t i=0; i<c->Nfft; i++) {
    for (size_t j=0; j<cal_Nsubopt; j++) {
      size_t t = router5(csub, j);
      t = (cal_ofs+t)%c->Ncalib;
      w=1;
      if ((c->calib_odd) &(cal_c%2==1)) w=-1;
      for (size_t cc=0; cc<c->Nchannels; cc++) calbuf[j][cc][t] += w*cdata[cc][i];
    }
    cal_ofs++;
    if (cal_ofs == c->Ncalib) {
      cal_ofs = 0;
      cal_c += 1;
      //std::cout<<cal_c<<" " <<cal_Ntot<<cal_Nsub <<std::endl;
      if (cal_c == cal_Ntot) {
	      done = true;
	      break;
      } 
      csub = cal_c / cal_Nsub;
    }
    if (done) break;
  }

  if (done) {
    size_t maxvar_i=-1;
    size_t minvar_i=-1;
  
    float maxvar = -1;
    float minvar = 1e100;
  
    for (size_t j=0;j<cal_Nsubopt;j++) {
      float cvar = 0;
      for (size_t i=0; i<c->Ncalib; i++) 
	      for (size_t cc=0;cc<c->Nchannels;cc++) cvar+=pow(calbuf[j][cc][i],2);
          //      std::cout << j << " " <<cvar<<std::endl;
        if (cvar>maxvar) {
	        maxvar = cvar;
	        maxvar_i = j;
        }

        if (cvar<minvar) {
	        minvar = cvar;
	        minvar_i = j;
        }
    }

    std::cout << "Calib Drift NDX : " << maxvar_i << " " <<"Det:"<<maxvar/minvar<<std::endl;
    

    for (size_t i=0; i<c->Ncalib; i++) 
      for (size_t cc=0; cc<c->Nchannels; cc++) res->calib_out[cc][i] = calbuf[maxvar_i][cc][i];
    res->calib_drift_count = maxvar_i;
    res->calib_drift_N = cal_Ntot;
    res->calib_det = maxvar/minvar;
    // reset
    zero_calbuf(); 
    cal_ofs = 0;
    cal_c = 0;
  }
  
  
}



void RefSpectrometer::run (SpecOutput *res) {

  SpecOutput  avg1(*res);
  res->zero();
  pfb_counter = 0;
  if (c->Ncalib > 0) zero_calbuf();
  cal_c = 0; // this is redundant here
  cal_ofs  = 0;
    
  for (size_t counter=0;counter<c->AverageSize();counter++) {
    assert (source->data_available());
    // get new block of data
    float* cdata[MAX_CHANNELS];
    source->next_block(cdata);
    run_pfb(cdata, res, &avg1);
    if (c->Ncalib > 0) run_calib(cdata, res);
  }
  
  if (res->Nradiometer>0) *res/=(float(res->Nradiometer)/2/c->Nfft*c->sampling_rate); 
}
  

void RefSpectrometer::process_output(SpecOutput *res) {

  
  // Notch filter
  if (c->notch) {
    for (size_t i=0;i<c->Nchannels;i++) {
      for (size_t k=0;k<res->Nbins;k++) {
	      fftwf_complex mean = {0.0 , 0.0};
	      for (size_t j=0;j<c->Average1Size;j++) {
	        mean[0] += pfb_out[j][i][k][0];
	        mean[1] += pfb_out[j][i][k][1];
	      }
	      mean[0] /= c->Average1Size;
	      mean[1] /= c->Average1Size;

        res->notch_out[i][k][0]=mean[0];
        res->notch_out[i][k][1]=mean[1];
        if (k==2)
          std::cout << res->notch_out[i][k][0] <<std::endl;
	      for (size_t j=0;j<c->Average1Size;j++) {
	        pfb_out[j][i][k][0] -= mean[0];
	        pfb_out[j][i][k][1] -= mean[1];
	      }
      }
    }
  }


  
  // Auto channels
  for (size_t i=0;i<c->Nchannels;i++) {
    for (size_t k=0;k<res->Nbins;k++) res->avg_pspec[i][k]=0.0;

    for (size_t j=0;j<c->Average1Size;j++) {
      for (size_t k=0;k<res->Nbins;k++) {
	      res->avg_pspec[i][k]+=
	      (pfb_out[j][i][k][0]*pfb_out[j][i][k][0] +
	      pfb_out[j][i][k][1]*pfb_out[j][i][k][1]);
      }
    }
      
    if (res->Nbins_zoom>0) {
      int Nzch = c->zoom_weights.size();
      int zif = c->zoom_weights[0].size();
      for (size_t k=0;k<res->Nbins_zoom;k++) res->avg_pspec_zoom[i][k]=0.0;
      for (size_t j=0;j<c->Average1Size/zif;j++) {

        std::vector<float> pfb_sample_re(res->Nbins_zoom,0.0);
        std::vector<float> pfb_sample_im(res->Nbins_zoom,0.0); 
        
        for (size_t k=0;k<res->Nbins_zoom;k++) {
          int sb = k%Nzch;
          int bb = k/Nzch+c->zoomin_st;
          pfb_sample_re[k] = 0.0;
          pfb_sample_im[k] = 0.0;
          for (int a=0;a<zif;a++) {
            pfb_sample_re[k] += pfb_out[j*zif+a][i][bb][0]*c->zoom_weights[sb][a] - pfb_out[j*zif+a][i][bb][1]*c->zoom_weights_imag[sb][a];
            pfb_sample_im[k] += pfb_out[j*zif+a][i][bb][0]*c->zoom_weights_imag[sb][a] + pfb_out[j*zif+a][i][bb][1]*c->zoom_weights[sb][a];
          }
        }       
        
        // this hocus pocus (attempt do deconolve in case of even number zoom)
        // doesn't work outside bin center due to parent window mismatch.
        /*for (size_t k=Nzch;k<res->Nbins_zoom-Nzch;k+=Nzch) {
          pfb_sample_re[k] = (pfb_sample_re[k]+pfb_sample_re[k+Nzch])/2;
          pfb_sample_im[k] = (+pfb_sample_im[k]+pfb_sample_im[k+Nzch])/2;
        }*/

        for (size_t k=0;k<res->Nbins_zoom;k++) {
          res->avg_pspec_zoom[i][k]+=
            (pfb_sample_re[k]*pfb_sample_re[k] +
             pfb_sample_im[k]*pfb_sample_im[k]);
        }
      } 
    }

  }
      
  // Now all the cross
  size_t cc = c->Nchannels; // We start with where we ended
  for (size_t i1=0; i1<c->Nchannels; i1++) {
    for (size_t i2=i1+1; i2<c->Nchannels; i2++) {
      for (size_t k=0;k<res->Nbins;k++) {
	res->avg_pspec[cc][k]=0.0;
	res->avg_pspec[cc+1][k]=0.0;
      }
      for (size_t j=0;j<c->Average1Size;j++) {
	for (size_t k=0;k<res->Nbins;k++) {
	  // real part
	  res->avg_pspec[cc][k]+=
	    (pfb_out[j][i1][k][0]*pfb_out[j][i2][k][0] +
	     pfb_out[j][i1][k][1]*pfb_out[j][i2][k][1]);
	  // imag part
	  res->avg_pspec[cc+1][k]+=
	    (pfb_out[j][i1][k][1]*pfb_out[j][i2][k][0] -
	     pfb_out[j][i1][k][0]*pfb_out[j][i2][k][1]);
	}
      }
      cc+=2;
    }
  }

  res->Nradiometer = c->Average1Size - int(c->notch);
  assert (cc == res->Nspec); 
}


  

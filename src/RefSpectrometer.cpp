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

  //std::cout<< c->AverageSize << " " <<c->Nchannels << " " <<pfb.get_Ncomplex() <<std::endl;
  //assert(0);

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
  std::cout << " Calib Average1  : " << cal_Nsub << " ~ " << cal_Nsub*c->Ncalib/c->sampling_rate*1e3<<"ms"<<std::endl;
  std::cout << " Calib Average2  : " << cal_Ntot/cal_Nsub << std::endl;
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
      norm += c->AverageSize();
      *res += *avg1;
      pfb_counter = 0;
    }
}

int router5 (size_t sub, size_t j) {
  size_t pm =0;
  const int res1[9] =  { 0, 0, 1, 1, 1, -1, -1, -1,  0 };
  const int res2[9] =  { 0, 0, 1, 2, 2, -2, -2, -1,  0 };
  const int res3[9] =  { 0, 1, 2, 2, 3, -3, -2, -2, -1 };
  const int res4[9] =  { 0, 1, 2, 3, 4, -4, -3, -2, -1 };

  switch (sub) {
  case 0:
    return 0;
  case 1:
    return res1[j];
  case 2:
    return res2[j];
  case 3:
    return res3[j];
  case 4:
    return res4[j];
  }
  return 0;
}

void RefSpectrometer::run_calib(float**cdata, SpecOutput *res)
{

  size_t  csub = cal_c % cal_Nsub;
  bool done = false;
  for (size_t i=0; i<c->Nfft; i++) {

    for (size_t j=0; j<cal_Nsubopt; j++) {
      size_t t = router5(csub, j);
      t = (cal_ofs+t)%c->Ncalib;
      for (size_t cc=0; cc<c->Nchannels; cc++) calbuf[j][cc][t] += cdata[cc][i];
    }

    cal_ofs++;
    if (cal_ofs == c->Ncalib) {
      cal_ofs = 0;
      cal_c += 1;
      if (cal_c == cal_Ntot) {
	done = true;
	break;
      } 
      csub = cal_c % cal_Nsub;
    }
    if (done) break;
  }

  if (done) {
    size_t maxvar_i=-1;
    float maxvar = -1;
    for (size_t j=0;j<cal_Nsubopt;j++) {
      float cvar = 0;
      for (size_t i=0; i<c->Ncalib; i++) 
	for (size_t cc=0;cc<c->Nchannels;cc++) cvar+=pow(calbuf[j][cc][i],2);
      std::cout <<j <<" ajja" <<cvar<<std::endl;
      if (cvar>maxvar) {
	maxvar = cvar;
	maxvar_i = j;
      }
    }
    std::cout <<maxvar_i <<" XX" <<std::endl;
    for (size_t i=0; i<c->Ncalib; i++) 
      for (size_t cc=0; cc<c->Nchannels; cc++) res->calib_out[cc][i] = calbuf[maxvar_i][cc][i];
    res->calib_drift_count = maxvar_i;
    res->calib_drift_N = cal_Ntot;
    // reset
    calbuf = Vec3(cal_Nsubopt,Vec2(c->Nchannels, Vec1(c->Ncalib,0.0)));
    cal_ofs = 0;
    cal_c = 0;
  }
  
  
}



void RefSpectrometer::run (SpecOutput *res) {

  SpecOutput  avg1(*res);
  res->zero();
  std::cout << res->calib_out[0] << " AA " <<std::endl;
  //if (nblocks == 0) nblocks = c->Average2Size;
  //if (c->Ncalib>0) assert(nblocks == c->Average2Size); // with calibration only this makes sense
  norm = 0;
  pfb_counter = 0;
  calbuf = Vec3(cal_Nsubopt,Vec2(c->Nchannels, Vec1(c->Ncalib,0.0)));
  cal_c = 0; // this is redundant here
  cal_ofs  = 0;
    
  for (size_t counter=0;counter<c->AverageSize();counter++) {
    if (!source->data_available()) break;
    // get new block of data
    float* cdata[MAX_CHANNELS];
    source->next_block(cdata);
    run_pfb(cdata, res,&avg1);
    if (c->Ncalib > 0) run_calib(cdata, res);
  }
  
  if (norm>0) *res/=(float(norm)*c->sampling_rate*sqrt(c->Nfft)/90.5153); //90 is a fudge
  res->Nradiometer = norm;
}
  

void RefSpectrometer::process_output(SpecOutput *res) {

    const float zoom_weights3[3][3][2]  = {{{0.3333333333333333, 0.0 },{-0.16666666666666666, 0.28867513459481287 },{-0.16666666666666666, -0.28867513459481287 }},
			    {{0.3333333333333333, 0.0 },{0.3333333333333333, 0.0 },{0.3333333333333333, 0.0 }},
			    {{0.3333333333333333, 0.0 },{-0.16666666666666666, -0.28867513459481287 },{-0.16666666666666666, 0.28867513459481287 }}};

    const float zoom_weights4[4][4][2] ={{{0.25, 0.0 },{0.0, -0.25 },{-0.25, 0.0 },{0.0, 0.25 }},
					 {{0.25, 0.0 },{0.25, 0.0 },{0.25, 0.0 },{0.25, 0.0 }},
					 {{0.25, 0.0 },{0.0, 0.25 },{-0.25, 0.0 },{0.0, -0.25 }},
					 {{0.25, 0.0 },{-0.25, 0.0 },{0.25, 0.0 },{-0.25, 0.0 }}};


    const float zoom_weights5[5][5][2] = {{{0.2, 0.0 },{-0.1618033988749895, -0.11755705045849463 },{0.06180339887498949, 0.1902113032590307 },{0.06180339887498949, -0.1902113032590307 },{-0.1618033988749895, 0.11755705045849463 }},
					  {{0.2, 0.0 },{0.06180339887498949, -0.1902113032590307 },{-0.1618033988749895, -0.11755705045849463 },{-0.1618033988749895, 0.11755705045849463 },{0.06180339887498949, 0.1902113032590307 }},
					  {{0.2, 0.0 },{0.2, 0.0 },{0.2, 0.0 },{0.2, 0.0 },{0.2, 0.0 }},
					  {{0.2, 0.0 },{0.06180339887498949, 0.1902113032590307 },{-0.1618033988749895, 0.11755705045849463 },{-0.1618033988749895, -0.11755705045849463 },{0.06180339887498949, -0.1902113032590307 }},
					  {{0.2, 0.0 },{-0.1618033988749895, 0.11755705045849463 },{0.06180339887498949, -0.1902113032590307 },{0.06180339887498949, 0.1902113032590307 },{-0.1618033988749895, -0.11755705045849463 }}};


    float zoom_weights[5][5][2];

    if (c->zoomin_fact>0) {
      int N = c->zoomin_fact;
      for (size_t i=0;i<N;i++) {
	for (size_t j=0;j<N;j++) {
	  for (size_t k=0;k<2;k++) {
	    if (N==3) zoom_weights[i][j][k] = sqrt(N)*zoom_weights3[i][j][k];
	    if (N==4) zoom_weights[i][j][k] = sqrt(N)*zoom_weights4[i][j][k];
	    if (N==5) zoom_weights[i][j][k] = sqrt(N)*zoom_weights5[i][j][k];
	  }
	}
      }
    }

    
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
      
    if (c->zoomin_fact>0) {
    for (size_t k=0;k<res->Nbins_zoom;k++) res->avg_pspec_zoom[i][k]=0.0;
      int zif = c->zoomin_fact; 
      for (size_t j=0;j<c->Average1Size/zif;j++) {
	for (size_t k=0;k<res->Nbins_zoom;k++) {
	  int sb = k%zif;
	  int bb = k/zif+c->zoomin_st;
	  float pfb_sample_re = 0;
	  float pfb_sample_im = 0;
	  for (size_t a=0;a<zif;a++) {
	    pfb_sample_re += pfb_out[j*zif+a][i][bb][0]*zoom_weights[sb][a][0] - pfb_out[j*zif+a][i][bb][1]*zoom_weights[sb][a][1];
	    pfb_sample_im += pfb_out[j*zif+a][i][bb][0]*zoom_weights[sb][a][1] + pfb_out[j*zif+a][i][bb][1]*zoom_weights[sb][a][0];
	  }
	  res->avg_pspec_zoom[i][k]+= pfb_sample_re*pfb_sample_re + pfb_sample_im * pfb_sample_im;
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

  assert (cc == res->Nspec); 
}
      
  

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

  pfb_out = new fftwf_complex**[c->AverageSize];
  for (size_t i=0;i<c->AverageSize;i++) {
    pfb_out[i] = new fftwf_complex*[c->Nchannels];
    for (size_t j=0;j<c->Nchannels;j++) pfb_out[i][j]=fftwf_alloc_complex(2*pfb.get_Ncomplex());
  }
  pfb.setup_plan(pfb_out[0][0]);

  // prefill buffer
  for (size_t i=1;i<c->Ntaps;i++) {
    float* cdata[MAX_CHANNELS];
    source->next_block(cdata);
    for (size_t j=0;j<c->Nchannels;j++) buffer[j][i]=cdata[j];
  }
  counter = 0;
}


void RefSpectrometer::run (SpecOutput *res, int nblocks) {

  SpecOutput  avg1(*res);
  res->zero();
  int avg1_counter = 0;
  int pfb_counter = 0;
  int norm = 0;
  if (nblocks == 0) nblocks = c->AverageSize;
  while (true) {
    // shift buffers by one unit
    for (size_t j=0;j<c->Nchannels;j++) {
      for (size_t i=0;i<c->Ntaps-1;i++) {
	buffer[j][i] = buffer[j][i+1];
      }
    }
     
    // get new block of data
    float* cdata[MAX_CHANNELS];
    if (!source->data_available()) break;
    source->next_block(cdata);
    for (size_t j=0;j<c->Nchannels;j++) {
      buffer[j][c->Ntaps-1] = cdata[j];
      pfb.process(buffer[j], pfb_out[pfb_counter][j]);
    }

    counter++;
    pfb_counter++;

    if (pfb_counter == c->AverageSize) {
      process_output(&avg1);
      norm += c->AverageSize;
      *res += avg1;
      pfb_counter = 0;
      avg1_counter++;
    }
    if (avg1_counter == nblocks) break;
  }
  
  if (norm>0) *res/=(float(norm)*c->sampling_rate*sqrt(c->Nfft)/90.5153); //90 is a fudge
  res->Nradiometer = norm;
}
  

void RefSpectrometer::process_output(SpecOutput *res) {
  //std::cout << "here" <<std::endl;
  //*res = SpecOutput(c);

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
	for (size_t j=0;j<c->AverageSize;j++) {
	  mean[0] += pfb_out[j][i][k][0];
	  mean[1] += pfb_out[j][i][k][1];
	}
	mean[0] /= c->AverageSize;
	mean[1] /= c->AverageSize;
	for (size_t j=0;j<c->AverageSize;j++) {
	  pfb_out[j][i][k][0] -= mean[0];
	  pfb_out[j][i][k][1] -= mean[1];
	}
      }
    }
  }


  
  // Auto channels
  for (size_t i=0;i<c->Nchannels;i++) {
    for (size_t k=0;k<res->Nbins;k++) res->avg_pspec[i][k]=0.0;

    for (size_t j=0;j<c->AverageSize;j++) {
      for (size_t k=0;k<res->Nbins;k++) {
	res->avg_pspec[i][k]+=
	  (pfb_out[j][i][k][0]*pfb_out[j][i][k][0] +
	   pfb_out[j][i][k][1]*pfb_out[j][i][k][1]);
      }
    }
      
    if (c->zoomin_fact>0) {
    for (size_t k=0;k<res->Nbins_zoom;k++) res->avg_pspec_zoom[i][k]=0.0;
      int zif = c->zoomin_fact; 
      for (size_t j=0;j<c->AverageSize/zif;j++) {
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
      for (size_t j=0;j<c->AverageSize;j++) {
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
      
  

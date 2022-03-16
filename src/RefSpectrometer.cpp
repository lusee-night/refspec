#include "RefSpectrometer.h"
#include "assert.h"
#include <iostream>

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

  int local_counter=0;
  while (true) {
      // shift buffers by one unit
      for (size_t j=0;j<c->Nchannels;j++) {
	for (size_t i=0;i<c->Ntaps-1;i++) {
	  buffer[j][i] = buffer[j][i+1];
	}
      }
     
      // get new block of data
      float* cdata[MAX_CHANNELS];
      source->next_block(cdata);
      for (size_t j=0;j<c->Nchannels;j++) {
	buffer[j][c->Ntaps-1] = cdata[j];
	pfb.process(buffer[j], pfb_out[local_counter][j]);
      }

      counter++;
      local_counter++;
      
      if (local_counter == nblocks) break;
      if (local_counter == c->AverageSize) {
	process_output(res);
	break;
      }
  }
}
  

void RefSpectrometer::process_output(SpecOutput *res) {
  //std::cout << "here" <<std::endl;
  //*res = SpecOutput(c);
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




  for (size_t i=0;i<c->Nchannels;i++) {
      for (size_t k=0;k<res->Nbins;k++) res->avg_pspec[i][k]=0.0;
      for (size_t j=0;j<c->AverageSize;j++) {
	for (size_t k=0;k<res->Nbins;k++) {
	  res->avg_pspec[i][k]+=
	       (pfb_out[j][i][k][0]*pfb_out[j][i][k][0] +
		pfb_out[j][i][k][1]*pfb_out[j][i][k][1]);
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
      
  

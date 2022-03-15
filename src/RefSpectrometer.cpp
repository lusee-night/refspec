#include "RefSpectrometer.h"
#include "assert.h"

RefSpectrometer::RefSpectrometer (SignalSource *source, SpecConfig const *config):
  source(source), c(config), pfb(c->sampling_rate, c->Nfft, c->Ntaps, c->window)
{
  buffer = new float**[c->Nchannels];

  // Now, we will allocate and copy/difference data only if required
  for (size_t i=0;i<c->Nchannels;i++) {
    if (c->minus_channel>=0) {
      assert(false); // not implemented
      //for (size_t j=0;j<c->Ntaps;j++) data_buffer[iS][j] = fftwf_alloc_real(c->Nfft);
    } else {
      assert (c->plus_channel>0);
    }
  }

  for (size_t i=0;i<c->Nchannels;i++) buffer[i] = new float*[c->Ntaps];
  pfb_out = new fftwf_complex**[c->BlockSize];
  for (size_t i=0;i<c->BlockSize;i++) {
    pfb_out[i] = new fftwf_complex*[c->Nchannels];
    for (size_t j=0;j<c->Nchannels;j++) pfb_out[i][j]=fftwf_alloc_complex(pfb.get_Ncomplex());
  }
  pfb.setup_plan(pfb_out[0][0]);

  // prefill buffer
  for (size_t i=1;i<c->Ntaps;i++) {
    float* cdata[MAX_CHANNELS];
    source->next_block(cdata);
    for (size_t j=0;i<c->Nchannels;j++) buffer[j][i]=cdata[j];
  }
  counter = 0;
}


SpecOutput RefSpectrometer::run (int nblocks) {


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
	pfb.process(buffer[j], pfb_out[counter][j]);
      }



      counter++;
    
    }

  
  
}
  

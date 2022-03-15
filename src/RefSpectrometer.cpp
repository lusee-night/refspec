#include "RefSpectrometer.h"
#include "assert.h"

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
  pfb_out = new fftwf_complex**[c->AverageSize];
  for (size_t i=0;i<c->AverageSize;i++) {
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

  SpecOutput toret=SpecOutput(c);
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
	pfb.process(buffer[j], pfb_out[counter][j]);
      }

      counter++;
      local_counter++;
      
      if (local_counter == nblocks) break;
      if (local_counter == c->AverageSize) {
	toret = process_output();
      }
  }
  return toret;
}
  

SpecOutput RefSpectrometer::process_output() {
    SpecOutput res(c);
    int Nbins = pfb.get_Ncomplex();
    for (size_t i=0;i<c->Nchannels;i++) {
      for (size_t j=0;j<Nbins;j++) res.avg_pspec[i][j]=0.0;
      for (size_t j=0;j<c->AverageSize;j++) {
	for (size_t k=0;k<Nbins;k++) res.avg_pspec[i][k]+=
	       (pfb_out[j][i][k][0]*pfb_out[j][i][k][0] +
		pfb_out[j][i][k][1]*pfb_out[j][i][k][1]);
      }
    }
    return res;
}
      
  

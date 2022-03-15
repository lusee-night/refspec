#pragma once

#include "SpecConfig.h"
#include "SpecOutput.h"
#include "SignalSource.h"

class RefSpectrometer{


 public:
  RefSpectrometer (SignalSource *source, SpecConfig const *config);

  // run for nblock, forever if 0
  SpecOutput run (int nblocks=0);

private:

  SignalSource *source;
  SpecConfig const *c;
  float ***buffer; // nchan x Ntaps x Nfft
  //  float *data_buffer[
  PolyphaseFilterBank pfb;
  fftwf_complex ***pfb_out; // BlockSize x Nchannels x Ncomplex
  size_t counter;

  SpecOutput process_output(); 


};
  

			 

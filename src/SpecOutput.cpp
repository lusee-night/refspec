#include "SpecOutput.h"
#include <assert.h>
#include <iostream>

SpecOutput::SpecOutput (SpecConfig const *config) : mode(config->mode),
    Nchannels(config->Nchannels), Nfft(config->Nfft), constructed(true) {
  //std::cout << "constructing"<<std::endl;
  Nspec = Nchannels*Nchannels; // think about matrix;
   Nbins = Nfft / 2 + 1;
   switch (mode) {
   case idle:
     break;
   case production:
     avg_pspec = new float*[Nspec];
     for (size_t i=0;i<Nspec;i++) 
       avg_pspec[i] = new float[Nbins];
     // add calibration thing
     break;
   default:
     assert(false); // not implemented
   }
}

SpecOutput::~SpecOutput() {
  //std::cout << "deconstructing"<<constructed<<std::endl;
  if (constructed) {
    switch (mode) {
    case idle:
      break;
    case production:
      for (size_t i=0;i<Nspec;i++) delete avg_pspec[i];
      delete avg_pspec;
   }
  }
}

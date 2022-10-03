#include "SpecOutput.h"
#include <assert.h>
#include <iostream>

SpecOutput::SpecOutput (SpecConfig const *config) : mode(config->mode),
    Nchannels(config->Nchannels), Nfft(config->Nfft), constructed(true) {
  //std::cout << "constructing"<<std::endl;
  Nspec = Nchannels*Nchannels; // think about matrix;
  Nbins = Nfft / 2 + 1;
  Nbins_zoom = (config->zoomin_en-config->zoomin_st) * config->zoomin_fact;
  switch (mode) {
  case idle:
    break;
  case production:
    avg_pspec = new float*[Nspec];
    if (Nbins_zoom>0) avg_pspec_zoom = new float*[Nspec];
    for (size_t i=0;i<Nspec;i++) {
      avg_pspec[i] = new float[Nbins];
      if (Nbins_zoom>0) avg_pspec_zoom[i] = new float[Nbins_zoom];
    }
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
      if (Nbins_zoom>0)
	for (size_t i=0;i<Nspec;i++) delete avg_pspec_zoom[i];
      
      delete avg_pspec;
   }
  }
}

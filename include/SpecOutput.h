#pragma once

#include "SpecConfig.h"

struct SpecOutput {

  spec_mode_t mode;
  size_t Nchannels, Nspec, Nbins, Nbins_zoom;
  size_t Nfft;

  float **avg_pspec;
  float **avg_pspec_zoom; //zoom region

  
  SpecOutput(): constructed (false) {};
  SpecOutput(SpecConfig const *config);
  ~SpecOutput();

private:
  bool constructed;
};

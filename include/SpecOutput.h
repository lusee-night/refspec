#pragma once

#include "SpecConfig.h"

class SpecOutput {

  SpecOutput(SpecConfig const *config);

  ~SpecOutput();
private:
  spec_mode_t mode;
  int8_t Nchannels, Nspec, Nbins;
  uint32_t Nfft;

  float **avg_pspec;
  // float **calib; //comes later

};

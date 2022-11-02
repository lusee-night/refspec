#pragma once

#include "SpecConfig.h"

struct SpecOutput {

  spec_mode_t mode;
  size_t Nchannels, Nspec, Nbins;
  size_t Nfft;

  float **avg_pspec;

  SpecOutput(): constructed (false) {};
  SpecOutput(SpecConfig const *config);
  ~SpecOutput();

  // Helper functions (just in case)
  size_t get_Nfft();
  int get_mode();

private:
  bool constructed;
};

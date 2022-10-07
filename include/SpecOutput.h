#pragma once

#include "SpecConfig.h"

struct SpecOutput {

  spec_mode_t mode;
  size_t Nchannels, Nspec, Nbins, Nbins_zoom;
  size_t Nfft;
  size_t Nradiometer;

  float **avg_pspec;
  float **avg_pspec_zoom; //zoom region

  
  SpecOutput(): constructed (false) {};
  SpecOutput(SpecConfig const *config);
  SpecOutput (SpecOutput const &S);


  void zero();
  SpecOutput& operator+=(SpecOutput& toadd);
  SpecOutput& operator/=(float V);

  ~SpecOutput();

private:
  void allocate(); 
  bool constructed;
};

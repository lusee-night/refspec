#pragma once

#include "SpecConfig.h"

struct SpecOutput {

  size_t Nchannels, Nspec, Nbins, Nbins_zoom, Ncalib;
  size_t Nfft;
  size_t Nradiometer;

  float **avg_pspec;
  float **avg_pspec_zoom; //zoom region

  float **calib_out;
  float calib_drift;
  int calib_drift_count, calib_drift_N;
  
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

#pragma once

#include "SpecConfig.h"

struct SpecOutput {

  size_t Nchannels, Nfft, Ncalib, Nradiometer, Nspec, Nbins, Nbins_zoom;


  float **avg_pspec;
  float **avg_pspec_zoom; //zoom region

  float **calib_out;
  float calib_drift, calib_det;
  int calib_drift_count, calib_drift_N;
  
  SpecOutput(): constructed (false) {};
  SpecOutput(SpecConfig const *config);
  SpecOutput (SpecOutput const &S);

  float get_avg_pspec(size_t i, size_t j) {return avg_pspec[i][j];};
  float get_calib_out(size_t i, size_t j) {return calib_out[i][j];};

  void zero();
  SpecOutput& operator+=(SpecOutput& toadd);
  SpecOutput& operator/=(float V);

  ~SpecOutput();

private:
  void allocate(); 
  bool constructed;
};

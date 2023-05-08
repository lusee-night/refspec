#pragma once

#include "SpecConfig.h"
#include <vector>

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



  float get_avg_pspec(size_t c, size_t bin) {return avg_pspec[c][bin];};
  float get_avg_pspec_zoom(size_t c, size_t bin) {return avg_pspec_zoom[c][bin];};
  std::vector<double> get_avg_pspec (size_t c);
  std::vector<double> get_avg_pspec_zoom (size_t c);
  float get_calib_out(size_t c, size_t bin) {return calib_out[c][bin];};

  void zero();
  SpecOutput& operator+=(SpecOutput& toadd);
  SpecOutput& operator/=(float V);

  ~SpecOutput();

private:
  void allocate(); 
  bool constructed;
};

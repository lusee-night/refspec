#pragma once

#include "SpecConfig.h"
#include <vector>
#include <fftw3.h>
#include <complex>

struct SpecOutput {

  size_t Nchannels, Nfft, Ncalib, Nradiometer, Nspec, Nbins, Nbins_zoom;


  float **avg_pspec;
  float **avg_pspec_zoom; //zoom region
  fftwf_complex **notch_out; // notch filter output (can be used for calibrator testing)

  float **calib_out;
  float calib_drift, calib_det;
  int calib_drift_count, calib_drift_N;
  
  SpecOutput(): constructed (false) {};
  SpecOutput(SpecConfig const *config);
  SpecOutput (SpecOutput const &S);



  float get_avg_pspec(size_t c, size_t bin) {return avg_pspec[c][bin];};
  float get_avg_pspec_zoom(size_t c, size_t bin) {return avg_pspec_zoom[c][bin];};
  std::complex<float> get_notch_out(size_t c, size_t bin) {return std::complex<float>(notch_out[c][bin][0],notch_out[c][bin][1]);};
  std::vector<double> get_avg_pspec (size_t c);
  std::vector<double> get_avg_pspec_zoom (size_t c);
  float get_calib_out(size_t c, size_t bin) {return calib_out[c][bin];};

  void zero();
  SpecOutput& operator+=(SpecOutput& toadd);
  SpecOutput& operator/=(float V);

  ~SpecOutput();

private:
  void allocate(bool do_notch); 
  bool constructed;
};

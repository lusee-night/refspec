#include "SpecOutput.h"
#include <assert.h>
#include <iostream>

SpecOutput::SpecOutput (SpecConfig const *config) :
  Nchannels(config->Nchannels), Nfft(config->Nfft),
  Ncalib(config->Ncalib), Nradiometer(0), constructed(true) {
  //std::cout << "constructing"<<std::endl;

  Nspec = Nchannels*Nchannels; // think about matrix;
  Nbins = Nfft / 2 + 1;
  Nbins_zoom = (config->zoomin_en-config->zoomin_st) * config->zoom_weights.size();
  allocate(config->notch);
}

SpecOutput::SpecOutput (SpecOutput const &S) :
  Nchannels(S.Nchannels), Nfft(S.Nfft), Ncalib(S.Ncalib), Nradiometer(S.Nradiometer),
  Nspec(S.Nspec), Nbins(S.Nbins), Nbins_zoom(S.Nbins_zoom),
  constructed(true)
{
  allocate((notch_out!=NULL));
}


void SpecOutput::allocate(bool do_notch) {
    avg_pspec = new float*[Nspec];
    if (Nbins_zoom>0) avg_pspec_zoom = new float*[Nspec];
    for (size_t i=0;i<Nspec;i++) {
      avg_pspec[i] = new float[Nbins];
      if (Nbins_zoom>0) avg_pspec_zoom[i] = new float[Nbins_zoom];
    }
    if (Ncalib>0) {
      calib_out = new float*[Nchannels];
      for (size_t i=0;i<Nchannels;i++) calib_out[i] = new float[Ncalib];
    }
    if (do_notch) {
      notch_out= new fftwf_complex*[Nchannels];
      for (size_t i=0;i<Nchannels;i++) notch_out[i] = new fftwf_complex[Nbins];
    } else notch_out = NULL;
}

void SpecOutput::zero() {
  for (size_t i=0;i<Nspec;i++) {
    for (size_t j=0;j<Nbins;j++) avg_pspec[i][j] = 0.0; 
    if (Nbins_zoom>0)
      for (size_t j=0;j<Nbins_zoom;j++) avg_pspec_zoom[i][j] = 0.0; 
  }
  Nradiometer = 0.0;
}


SpecOutput& SpecOutput::operator+=(SpecOutput& toadd) {
  assert(constructed);
  for (size_t i=0;i<Nspec;i++) {
    for (size_t j=0;j<Nbins;j++) avg_pspec[i][j] += toadd.avg_pspec[i][j]; 
    if (Nbins_zoom>0)
      for (size_t j=0;j<Nbins_zoom;j++) avg_pspec_zoom[i][j] += toadd.avg_pspec_zoom[i][j]; 
  }
  Nradiometer += toadd.Nradiometer;
  return *this;
}

SpecOutput& SpecOutput::operator/=(float V) {
  assert(constructed);
  for (size_t i=0;i<Nspec;i++) {
    for (size_t j=0;j<Nbins;j++) avg_pspec[i][j] /= V;
    if (Nbins_zoom>0)
      for (size_t j=0;j<Nbins_zoom;j++) avg_pspec_zoom[i][j] /= V;
  }
  return *this;
}

std::vector<double> SpecOutput::get_avg_pspec (size_t c) {
  std::vector<double> out(Nbins);
  for (size_t i=0;i<Nbins;i++) out[i] = avg_pspec[c][i];
  return out;
}

std::vector<double> SpecOutput::get_avg_pspec_zoom (size_t c) {
  std::vector<double> out(Nbins_zoom);
  for (size_t i=0;i<Nbins_zoom;i++) out[i] = avg_pspec_zoom[c][i];
  return out;
}



SpecOutput::~SpecOutput() {
  //std::cout << "deconstructing"<<constructed<<std::endl;
  if (constructed) {
    for (size_t i=0;i<Nspec;i++) delete avg_pspec[i];
    delete avg_pspec;
    if (Nbins_zoom>0) {
      for (size_t i=0;i<Nspec;i++) delete avg_pspec_zoom[i];
      delete avg_pspec_zoom;
    }
    if (Ncalib>0) {
      for (size_t i=0;i<Nchannels;i++) delete calib_out[i];
      delete calib_out;
    }
    if (notch_out) {
      for (size_t i=0;i<Nchannels;i++) delete notch_out[i];
      delete notch_out;}
  }
}

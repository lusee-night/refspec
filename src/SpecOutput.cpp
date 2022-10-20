#include "SpecOutput.h"
#include <assert.h>
#include <iostream>

SpecOutput::SpecOutput (SpecConfig const *config) :
  Nchannels(config->Nchannels), Nfft(config->Nfft),
  Ncalib(config->Ncalib), Nradiometer(0), constructed(true) {
  //std::cout << "constructing"<<std::endl;
  Nspec = Nchannels*Nchannels; // think about matrix;
  Nbins = Nfft / 2 + 1;
  Nbins_zoom = (config->zoomin_en-config->zoomin_st) * config->zoomin_fact;
  allocate();
}

SpecOutput::SpecOutput (SpecOutput const &S) :
  Nchannels(S.Nchannels), Nfft(S.Nfft), constructed(true),
  Nspec(S.Nspec), Nbins(S.Nbins), Nbins_zoom(S.Nbins_zoom), Nradiometer(S.Nradiometer),
  Ncalib(S.Ncalib)
{
  allocate();
}


void SpecOutput::allocate() {
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
    std::cout << Ncalib << " " << Nchannels <<" " <<calib_out[0] << "cc"<<avg_pspec[0] <<std::endl;
}

void SpecOutput::zero() {
    for (size_t i=0;i<Nspec;i++) {
      for (size_t j=0;j<Nbins;j++) avg_pspec[i][j] = 0.0; 
      if (Nbins_zoom>0)
	for (size_t j=0;j<Nbins_zoom;j++) avg_pspec_zoom[i][j] = 0.0; 
    }
}


SpecOutput& SpecOutput::operator+=(SpecOutput& toadd) {
  assert(constructed);
  for (size_t i=0;i<Nspec;i++) {
    for (size_t j=0;j<Nbins;j++) avg_pspec[i][j] += toadd.avg_pspec[i][j]; 
    if (Nbins_zoom>0)
      for (size_t j=0;j<Nbins_zoom;j++) avg_pspec_zoom[i][j] += toadd.avg_pspec_zoom[i][j]; 
  }
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
  }
}

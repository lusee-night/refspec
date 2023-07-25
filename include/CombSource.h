#pragma once

#include "SignalSource.h"
#include <cstdint>
#include <stddef.h>
#include <string>

class CombSource: public SignalSource {

public:
  CombSource(size_t block_size, size_t Nchannels,
	     size_t Nsamples,  std::string filename,
	     size_t oversample = 1,
	     float A0 = 1.0, float beta_A = 0,
	     float alpha_t = 0.0, float beta_t = 0.0,
	     size_t sample_shift = 0, bool zero_mean = false);

  ~CombSource();
  
  virtual void next_block(float **place);

private:
  float *buffer; // stores output
  float *buffer_raw; // stores raw sample
  size_t Nsamples, Nsamples_raw;
  size_t oversample;
  float A0, beta_A, alpha_t, beta_t;
  
  size_t cc ;
  bool simple_copy;
};

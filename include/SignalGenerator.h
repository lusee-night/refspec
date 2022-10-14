#pragma once

#include "SignalSource.h"
#include <cstdint>
#include <stddef.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;

class SignalGenerator: public SignalSource {

public:
  SignalGenerator(size_t block_size, size_t Nchannels, size_t Nblocks, float frequency,
	       float sampling_rate, float Ampl, float noise_rms);

  ~SignalGenerator();
  
  virtual void next_block(float **place); 

private:
  float *buffer;
  size_t Nblocks;
  double frequency, sampling_rate, Ampl, noise_rms;
  double phase_offset;
  size_t cur_block ;
  void fill_buffer();
  
};

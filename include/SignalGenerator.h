#pragma once

#include "SignalSource.h"
#include <cstdint>
#include <stddef.h>

class SignalGenerator: public SignalSource {

  SignalGenerator(uint32_t block_size, uint32_t Nblocks, float frequency,
	       float sampling_rate, float Ampl, float noise_rms);

  virtual float* next_block(); 

 private:
  float *buffer;
  uint32_t Nblocks;
  double frequency, sampling_rate, Ampl, noise_rms;
  double phase_offset;
  size_t cur_block ;
  void fill_buffer();
  
};

#pragma once

#include "SignalSource.h"
#include <random>

class WhiteNoise: public SignalSource {

public:
  WhiteNoise(size_t block_size, size_t Nchannels,  float rms);
  ~WhiteNoise();
  
  virtual void next_block(float **place); 

private:
  float *buffer;
  std::default_random_engine generator;
  std::normal_distribution<double> gauss;

};

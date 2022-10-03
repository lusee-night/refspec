#pragma once

#include "SignalSource.h"
#include <cstdint>
#include <stddef.h>
#include <string>
#include <vector>

class PowerSpecSource: public SignalSource {

public:


  PowerSpecSource(const std::vector<double> &kk,
		  const std::vector<double> &Pk,
		  float sampling_rate,
		  size_t block_size, size_t Nchannels,
		  size_t Nblocks_gen, bool repeat = false, bool second_fourier=false);

  PowerSpecSource(std::string filename, float sampling_rate, size_t block_size,
		  size_t Nchannels, size_t Nblocks_gen, bool repeat = false,
		  bool second_fourier=false);

  ~PowerSpecSource();
  
  virtual void next_block(float **place); 

private:

  void generate_data(const std::vector<double> &kk, const std::vector<double> &Pk);
  
  float *buffer;
  float sampling_rate;
  bool repeat, second_fourier;
  size_t cur_block, Nblocks;
};


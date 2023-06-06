#pragma once

#include "SignalSource.h"
#include <cstdint>
#include <stddef.h>
#include <string>
#include <vector>

typedef std::vector<std::vector<std::vector<complex>>> CholKMatrix;

class CorrelatedSpecSource: public SignalSource {

public:


  CorrelatedSpecSource(const std::vector<double> &kk,
		  const CholKMatrix &C,
		  float sampling_rate,
		  size_t block_size, size_t Nchannels,
		  size_t Nblocks_gen, bool repeat = false, bool second_fourier=false,
		  int seed=123, bool verb=false);

  CorrelatedSpecSource(std::string filename, float sampling_rate, size_t block_size,
		  size_t Nchannels, size_t Nblocks_gen, bool repeat = false,
		  bool second_fourier=false, int seed=123, bool verb=false);

  ~CorrelatedSpecSource();
  
  virtual void next_block(float **place);
  virtual void next_block();
  virtual bool data_available() const;

private:

  void generate_data(const std::vector<double> &kk, const CholKMatrix C, int seed);
  
  std::vector<float*> buffer;
  float sampling_rate;
  bool repeat, second_fourier;
  size_t cur_block, Nblocks;
};


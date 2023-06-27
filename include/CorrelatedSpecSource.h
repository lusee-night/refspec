#pragma once

#include "SignalSource.h"
#include <cstdint>
#include <stddef.h>
#include <string>
#include <vector>
#include <cmath>
#include <complex>

typedef std::complex<double> fcomplex;
typedef std::vector<std::vector<fcomplex>> CholMatrix;

// CholKMatrix is Nfrequency x Nchannel x Nchannels in size
typedef std::vector<CholMatrix> CholKMatrix;

class CorrelatedSpecSource: public SignalSource {

public:


  CorrelatedSpecSource(const std::vector<double> &kk,
		  const CholKMatrix &C,
		  float sampling_rate,
		  size_t block_size, size_t Nchannels,
		  size_t Nblocks_gen, bool repeat = false, bool second_fourier=false,
		  int seed=123, bool verb=false);


  // to deal with python interface
  CorrelatedSpecSource(const std::vector<double> &kk,
		  float sampling_rate,
		  size_t block_size, size_t Nchannels,
		  size_t Nblocks_gen, bool repeat = false, bool second_fourier=false,
		  int seed=123, bool verb=false);
    void _set_internal_C (size_t i, size_t j, size_t k, fcomplex val); 
    void _go_internal(const std::vector<double> &kk,int seed);

  CorrelatedSpecSource(std::string filename, float sampling_rate, size_t block_size,
		  size_t Nchannels, size_t Nblocks_gen, bool repeat = false,
		  bool second_fourier=false, int seed=123, bool verb=false);



  virtual ~CorrelatedSpecSource();
  
  virtual void next_block(float **place);
  virtual void next_block();
  virtual bool data_available() const;

private:

  void generate_data(const std::vector<double> &kk, const CholKMatrix &C, int seed);
  
  std::vector<float*> buffer;
  float sampling_rate;
  bool repeat, second_fourier;
  size_t cur_block, Nblocks;

  CholKMatrix Cint;
};


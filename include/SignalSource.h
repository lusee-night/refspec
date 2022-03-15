#pragma once
#include <cstdint>
#include "SpecConfig.h"

class SignalSource {

 public:
  SignalSource(size_t block_size, size_t nchannels) :
    block_size(block_size), nchannels(nchannels){}

  virtual void next_block(float **place); 

  size_t get_block_size() const {return block_size;}
  
 protected:
  size_t block_size, nchannels;

};

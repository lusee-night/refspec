#pragma once
#include <cstdint>
#include "SpecConfig.h"

class SignalSource {

 public:

  SignalSource() : Nchannels(0) {};
  SignalSource(size_t block_size, size_t Nchannels) :
    block_size(block_size), Nchannels(Nchannels){}

  virtual void next_block(float **place) = 0; 

  size_t get_block_size() const {return block_size;}
  size_t get_Nchannels() const {return Nchannels;}

  virtual bool data_available() const {return true;}
  
 protected:
  size_t block_size, Nchannels;

};

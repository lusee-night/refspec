#pragma once
#include <cstdint>


class SignalSource {

 public:
  SignalSource(uint32_t block_size); 

  virtual float* next_block(); 

  uint32_t get_block_size() const {return block_size;}
  
 protected:
  uint32_t block_size;

};

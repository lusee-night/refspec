#pragma once
#include <cstdint>
#include "SpecConfig.h"

// #include <iostream>
// using namespace std;

class SignalSource {

 public:

  SignalSource() : Nchannels(0), internal(NULL), Ninternal(0) {};
  SignalSource(size_t block_size, size_t Nchannels) :
    block_size(block_size), Nchannels(Nchannels), internal(NULL), Ninternal(0) {}

  virtual void next_block(float **place) = 0; 
  virtual void next_block() = 0;

  void init_internal(int N) {
    internal = new float*[N];
    Ninternal = N;
  }
  bool has_internal(void) {return (internal!=NULL);}
  int get_Ninternal() {return Ninternal;}

  size_t get_block_size() const { return block_size;}
  size_t get_Nchannels()  const { return Nchannels;}

  virtual bool data_available() const {return true;}

 protected:
  size_t block_size, Nchannels;
  float **internal;
  int Ninternal;
};

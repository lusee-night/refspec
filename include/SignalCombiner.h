#pragma once
#include <cstdint>
#include <vector>
#include "SignalSource.h"

class SignalCombiner: public SignalSource {

 public:
  SignalCombiner(std::vector<SignalSource*> sources);

  ~SignalCombiner();

  virtual void next_block(float **place);

  virtual bool data_available() const;
  
 protected:
  size_t block_size, Nchannels, Nsources;
  std::vector <SignalSource*> sources;
  float **buf;
  float **inbuf;
};

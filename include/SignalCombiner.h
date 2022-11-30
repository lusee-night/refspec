#pragma once
#include <cstdint>
#include <vector>
#include "SignalSource.h"

class SignalCombiner: public SignalSource {

 public:
  SignalCombiner(std::vector<SignalSource*> sources, bool calc_rms = false);

  ~SignalCombiner();

  virtual void next_block(float **place);
  virtual bool data_available() const;

  std::vector<double> rms() const;  

  
 protected:
  size_t block_size, Nchannels, Nsources;
  std::vector <SignalSource*> sources;
  float **buf;
  float **inbuf;
  bool calc_rms;
  std::vector<double> var;
  size_t rms_counter;
};

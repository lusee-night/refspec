#pragma once
#include <cstdint>
#include <vector>
#include <iostream>

#include "SignalSource.h"
#include "CombSource.h"

using namespace std;

class SignalCombiner: public SignalSource {

 public:
  SignalCombiner(std::vector<SignalSource*> sources, bool calc_rms = false);
  SignalCombiner(bool calc_rms = false);
  // SignalCombiner(std::vector<CombSource*> sources, bool calc_rms = false);  

  ~SignalCombiner();

  virtual void next_block(float **place);
  virtual bool data_available() const;

  std::vector<double> rms() const;  

  static std::vector<CombSource*> source_buffer;

  static std::vector<SignalSource*> conv(std::vector<CombSource*> ss) {
    cout << "hi " << ss.size() << endl;

    std::vector<SignalSource*> v;
    v.push_back(ss[0]);
    return v;
  };

 protected:
  size_t block_size, Nchannels, Nsources;

  std::vector <SignalSource*> sources;



  float **buf;
  float **inbuf;
  bool calc_rms;
  std::vector<double> var;
  size_t rms_counter;

};

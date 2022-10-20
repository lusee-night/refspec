#pragma once

#include "SpecConfig.h"
#include "SpecOutput.h"
#include "SignalSource.h"
#include <vector>

typedef std::vector<float> Vec1;
typedef std::vector<Vec1> Vec2;
typedef std::vector<Vec2> Vec3;


class RefSpectrometer{


 public:
  RefSpectrometer (SignalSource *source, SpecConfig const *config);

  // run for nblock, forever if 0
  void run (SpecOutput *res);

  //print info
  void print_info(); 
  size_t blocks_processed() { return counter; }
  
private:

  SignalSource *source;
  SpecConfig const *c;


  // PFB part
  float ***buffer; // nchan x Ntaps x Nfft
  PolyphaseFilterBank pfb;
  fftwf_complex ***pfb_out; // BlockSize x Nchannels x Ncomplex
  size_t counter, pfb_counter;
  int norm;

  // calib part
  size_t cal_c, cal_ofs,  cal_Ntot, cal_Nsub, cal_Nsubopt;
  Vec3 calbuf;
  
  
  void run_pfb(float**cdata, SpecOutput *res, SpecOutput *avg1);
  void run_calib(float**cdata, SpecOutput *res);
  void process_output(SpecOutput *res); 



};
  

			 

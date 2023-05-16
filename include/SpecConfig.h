#pragma once
#include "pfb.h"

#include <string>
#include <vector>
#include <cstdint>


#define MAX_CHANNELS 4
#define MAX_TAPS 128


struct  SpecConfig {

  // number of channels and config
  size_t Nchannels;
  int plus_channel[MAX_CHANNELS]; // ADC for channel i takes plus_channel[i]-minus_channel[i]
  int minus_channel[MAX_CHANNELS]; // input to minus. Use -1 for ground;
  

  // PFB engine setup
  double sampling_rate;
  size_t Nfft;
  size_t Nbins() const { return Nfft/2 +1; } // number of frequency bins
  double fundamental_frequency() const {return sampling_rate/Nfft;}

  size_t Ntaps;
  window_t window;

  // average sizes
  uint32_t Average1Size; // first level averaging with notch (<10ms)
  uint32_t Average2Size; // second level averaging to get to 10s Hz. B
  uint32_t AverageSize() const {return Average1Size * Average2Size;}  // product of the two
  uint32_t MinGenSize() const {return AverageSize() + Ntaps;} 
  
  // calibrator detector_setup
  size_t Ncalib;
  bool   calib_odd; // use calibrator 
  size_t calib_subint; // sub integration for clock tracking
  
  // notch filter for picket fence
  bool notch;

  // zoom-in size
  uint32_t zoomin_st, zoomin_en; // start end end of the zoom in region, C counting
  std::vector<std::vector<float>> zoom_weights, zoom_weights_imag; // zoom in factor
  virtual ~SpecConfig() {} ;

 public:
  // default constructor with some same defaults
  SpecConfig();
  void sanity_check() const;
  void resize_zoom(size_t Nzoom, size_t Nweights);
  void set_zoom_weight (size_t i, size_t j, float value_real, float value_imag);  
};
 
 

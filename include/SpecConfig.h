#pragma once
#include "pfb.h"

#include <string>
#include <cstdint>

#define MAX_CHANNELS 4
#define MAX_CALIB_SIGS 2
#define MAX_TAPS 128


// enumeration of possible modes spectrometer can be in 
enum spec_mode_t {
			idle,   // eat data and do nothing, minimize power
			self_test, // TBD 
			production,  // normal mode with averaging
			raw_input,  // send back raw_input waveforms
			raw_pfb,   // send back raw_PFB data
			shutdown  // controlled shutdown
};


struct  SpecConfig {

  // spectrometer mode
  spec_mode_t mode;

  // number of channels and config
  size_t Nchannels;
  int plus_channel[MAX_CHANNELS]; // ADC for channel i takes plus_channel[i]-minus_channel[i]
  int minus_channel[MAX_CHANNELS]; // input to minus. Use -1 for ground;
  

  // PFB engine setup
  double sampling_rate;
  size_t Nfft;
  size_t Ntaps;
  window_t window;

  // average size
  uint32_t AverageSize; 

  // zoom-in size
  uint32_t zoomin_st, zoomin_en; // start end end of the zoom in region, C counting
  uint32_t zoomin_fact; // zoom in factor
  
  // calibrator detector_setup
  size_t Ncalib;
  size_t calibrator_cycles [MAX_CALIB_SIGS];


  // notch filter for picket fence
  bool notch;

 public:
  // default constructor with some same defaults
  SpecConfig();

  double fundamental_frequency() const {return sampling_rate/Nfft;} 
  size_t Nbins() const { return Nfft/2 +1; } // number of frequency bins

};
 
  

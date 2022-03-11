#pragma once

#define MAX_CHANNELS 4
#define MAX_CALIB_SIGS 2

// enumeration of possible modes spectrometer can be in 
enum spec_mode_t {
			idle,   // eat data and do nothing, minimize power
			self_test, // TBD 
			production,  // normal mode with averaging
			raw_input,  // send back raw_input waveforms
			raw_pfb,   // send back raw_PFB data
			shutdown  // controlled shutdown
};


class SpecConfig {

 public:
  LuSEESpecConfig();

  
 private:

  // spectrometer mode
  spec_mode_t mode;
  
  // number of channels and config
  int8_t nchannels;
  int8_t plus_channel[MAX_CHANNELS]; // ADC for channel i takes plus_channel[i]-minus_channel[i]
  int8_t minus_channel[MAX_CHANNELS]; // input to minus. Use -1 for ground;
  

  // PFB engine setup
  double sampling_rate;
  uint32_t Nfft;
  uint16_t Ntaps;
  window_t window;

  // average
  uint32_t average_samples;
  
  // calibrator detector_setup
  uint32_t calibrator_cycles [MAX_CALIB_SIGS];
  
}
 
  

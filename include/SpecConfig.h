#pragma once
#include "pfb.h"

#include <string>
#include <cstdint>
#include <iostream>

#define MAX_CHANNELS 4
#define MAX_CALIB_SIGS 2
#define MAX_TAPS 128

using namespace::std;


// Basic build for testing purposes
// g++ -I../include -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) SpecConfig.cpp -o example$(python3.10-config --extension-suffix)

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
  size_t      Nchannels;
  int         plus_channel[MAX_CHANNELS]; // ADC for channel i takes plus_channel[i]-minus_channel[i]
  int         minus_channel[MAX_CHANNELS]; // input to minus. Use -1 for ground;
  

  // PFB engine setup
  double      sampling_rate;
  size_t      Nfft;
  size_t      Ntaps;
  window_t    window;

  // average size
  uint32_t    AverageSize; 
  
  // calibrator detector_setup
  size_t      Ncalib;
  size_t      calibrator_cycles [MAX_CALIB_SIGS];


  // notch filter for picket fence
  bool notch;

 public:
  // default constructor with some same defaults
  SpecConfig();

  size_t      get_Ntaps();

  double fundamental_frequency() const {return sampling_rate/Nfft;} 
  size_t Nbins() const { return Nfft/2 +1; } // number of frequency bins

};



/*
extern "C" {
    SpecConfig* sc_new() {return new SpecConfig();}

    unsigned long sc_Nfft(SpecConfig* sc)     { return sc->Nfft; }

    double sc_sampling_rate(SpecConfig* sc)   { return sc->sampling_rate; }

}
*/


/* ATTIC


class SX {
  public:
    SX() {
      this->Nfft = 7;
    };
    int Nfft;
};

extern "C" {
    SX* sx_new() {return new SX();}
    int sx_Nfft(SX* sx) {
      int x = sx->Nfft;
      x = 2;
      cout<<sx;
      return x; // static_cast<int>(x);
    }
}
*/
 
  

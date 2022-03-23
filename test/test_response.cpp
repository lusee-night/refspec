
#include "RefSpectrometer.h"
#include "SignalGenerator.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>


int main(int argc, char *argv[]) {

  if (argc!=3) {
    std::cout << "Specify taps_start taps_end at the command line." <<std::endl;
    return 1;
  }

  int taps_start = atoi(argv[1]);
  int taps_end = atoi(argv[2]);
  size_t notch_start = 0;
  size_t notch_end = 1;
  size_t win_start = 0;
  size_t win_end = 4;
  bool CI_mode = false; 
  
  if (taps_start <0) {
    std::cout << "CI test" <<std::endl;
    taps_start = taps_end = 4;
    notch_start = notch_end = 0;
    win_start = win_end = 3;
    CI_mode = true;
  }
  
  SpecConfig cfg;

  cfg.Ntaps       = 7;
  cfg.Nchannels   = 1;
  cfg.AverageSize = 64;

  double fundamental = cfg.fundamental_frequency();

  // Let's do 1 second worth of data
  size_t blocks   = cfg.AverageSize+2*cfg.Ntaps;
  float Ampl      = 10;
  float noiseA    = 0.0;
  double central  = 10; // doesn't really matter

  for (size_t Ntaps=taps_start; Ntaps<=taps_end; Ntaps++) {
    std::cout << "Doing taps: " << Ntaps << std::endl;
    for (int notch = notch_start; notch <= notch_end; notch++) {
       for (int win = win_start; win <= win_end; win++) {
        std::stringstream fname;
	if (CI_mode) 
	  fname << "response_test.dat";
	else
	  fname << "response_" << Ntaps<< "_" << notch << "_" << win << ".dat";
        std::ofstream outfile;
        outfile.open(fname.str());
        cfg.Ntaps = Ntaps;
        cfg.notch = notch;
	cfg.window = window_t(win);
        for (double freq = central -3 ; freq< central + 3; freq+=0.01) {
	  SignalGenerator signal(cfg.Nfft, cfg.Nchannels, blocks, freq*fundamental, cfg.sampling_rate, Ampl, noiseA);
	  RefSpectrometer S(&signal,&cfg);    
          SpecOutput O(&cfg);
	        S.run(&O);
	        outfile <<freq-central << " " << O.avg_pspec[0][int(central)] << std::endl;
        }
        outfile.close();
      } 
    } 
  } 
  return 0;
}

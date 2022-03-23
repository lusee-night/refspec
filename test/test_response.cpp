
#include "RefSpectrometer.h"
#include "SignalGenerator.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>


int main() {
  SpecConfig cfg;

  cfg.Ntaps = 7;
  cfg.Nchannels = 1;
  cfg.AverageSize = 64;
  double fundamental = cfg.fundamental_frequency();
  // Let's do 1 second worth of data
  size_t blocks = cfg.AverageSize+2*cfg.Ntaps;
  float Ampl = 10;
  float noiseA = 0.0;
  double central = 10; // doesn't really matter

  for (size_t Ntaps=20;Ntaps<21;Ntaps++) {
    for (int notch = 0; notch < 2; notch++) {
      std::stringstream fname;
      fname << "response_" << Ntaps<<"_"<<notch<<".dat";
      std::ofstream outfile;
      outfile.open(fname.str());

      cfg.Ntaps = Ntaps;
      cfg.notch = notch;
			 
	
      for (double freq = central -3 ; freq< central + 3; freq+=0.01) {
  
	SignalGenerator signal(cfg.Nfft, cfg.Nchannels, blocks,
			       freq*fundamental, cfg.sampling_rate, Ampl, noiseA);
	RefSpectrometer S(&signal,&cfg);
	SpecOutput O(&cfg);
	S.run(&O);
	outfile <<freq-central << " " << O.avg_pspec[0][10] << std::endl;
      }
      outfile.close();
    }
  }


  
  return 0;
}

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

  cfg.Ntaps           = 8;
  cfg.Nchannels       = 1;
  cfg.Average1Size     = 64;
  cfg.Average2Size     = 1;
  double fundamental  = cfg.fundamental_frequency();
  
  size_t blocks       = cfg.AverageSize()+2*cfg.Ntaps;
  float Ampl          = 10;
  float noiseA        = 0.0;
  
  SignalGenerator signal(cfg.Nfft, cfg.Nchannels, blocks,
			 10e6, cfg.sampling_rate, Ampl, noiseA);
  SpecOutput O(&cfg);
  RefSpectrometer S(&signal,&cfg);
  S.run(&O);

  for (size_t i=1;i<cfg.Nbins();i++) {
    std::cout << fundamental*i/1e6 << " " << O.avg_pspec[0][i] << std::endl; 
  }
  
  return 0;
}

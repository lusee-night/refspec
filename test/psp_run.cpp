#include "RefSpectrometer.h"
#include "FileStreamSource.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define NFFT  1024

int main() {
  SpecConfig cfg;

  cfg.Nfft            = NFFT;
  cfg.sampling_rate   = 38.4e6;
  cfg.Ntaps           = 4;
  cfg.Nchannels       = 1;
  cfg.Average1Size    = 32768/cfg.Nfft;
  cfg.Average2Size    = 1;
  cfg.notch           = true;
  double fundamental  = cfg.fundamental_frequency();

  double ps_avg[NFFT], ps_avg_notch[NFFT];

  for (size_t i=0;i<NFFT;i++) {
    ps_avg[i] = ps_avg_notch[i] = 0.0;
  }


  for (int notch = 0; notch<2; notch++) {
    cfg.notch = bool(notch);
    for (size_t i=0;i<8;i++) {
      std::ostringstream fname;
      fname << "data/PSP/PSP_raw_"<<i<<".bin";
      FileStreamSource signal(cfg.Nfft, cfg.Nchannels, fname.str());
      SpecOutput O(&cfg);
      RefSpectrometer S(&signal,&cfg);
      S.run(&O); // run twice to get rid fo transients;
      S.run(&O);
      for (size_t j=0;j<NFFT;j++) {
	if (notch) ps_avg_notch[j]+=O.avg_pspec[0][j]; else
		   ps_avg[j]+=O.avg_pspec[0][j];
      }
    }
  }


  for (size_t i=1;i<cfg.Nbins();i++) {
    std::cout << fundamental*i/1e6 << " " << ps_avg[i] << " " << ps_avg_notch[i] << std::endl; 
  }
  
  return 0;
}

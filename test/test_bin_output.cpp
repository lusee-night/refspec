#include "RefSpectrometer.h"
#include "FileStreamSource.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define NFFT 4096

int main() {
  SpecConfig cfg;

  cfg.Nfft            = NFFT;
  cfg.sampling_rate   = 102.4e6;
  cfg.Ntaps           = 4;
  cfg.Nchannels       = 1;
  cfg.Average1Size    = 5;
  cfg.Average2Size    = 1;
  cfg.notch           = false;
  double fundamental  = cfg.fundamental_frequency();

  double ps_avg[NFFT], ps_avg_notch[NFFT];

  for (size_t i=0;i<NFFT;i++) {
    ps_avg[i] = ps_avg_notch[i] = 0.0;
  }


  for (int notch = 0; notch<2; notch++) {
    cfg.notch = bool(notch);
      FileStreamSource signal(cfg.Nfft, cfg.Nchannels, "data/test/sky_pf_100.bin");
      SpecOutput O(&cfg);
      RefSpectrometer S(&signal,&cfg);
      S.run(&O);
      for (size_t j=0;j<cfg.Nfft;j++) {
	if (notch) ps_avg_notch[j]+=O.avg_pspec[0][j]; else
		   ps_avg[j]+=O.avg_pspec[0][j];
      }
  }


  for (size_t i=1;i<cfg.Nbins();i++) {
    std::cout << fundamental*i/1e6 << " " << ps_avg[i] << " " << ps_avg_notch[i] << std::endl; 
  }
  
  return 0;
}

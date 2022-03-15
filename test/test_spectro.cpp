
#include "RefSpectrometer.h"
#include "SignalGenerator.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>


int main() {
  SpecConfig cfg;
  auto Nfft = cfg.get_Nfft();
  auto Nchan = cfg.get_Nchannels();
  auto sampling_rate = cfg.get_sampling_rate();
  SignalGenerator signal(Nfft,Nchan, 512, 5e6, sampling_rate, 10, 0.0);
  RefSpectrometer S(&signal,&cfg);
  S.run();
  return 0;
}


#include "RefSpectrometer.h"
#include "SignalGenerator.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <chrono>


int main() {
  SpecConfig cfg;

  cfg.Ntaps = 8;
  cfg.Nchannels = 4;

  double fundamental = cfg.fundamental_frequency();
  // Let's do 1 second worth of data
  size_t blocks = int(cfg.sampling_rate/cfg.Nfft);
  float Ampl = 0;
  float noiseA = 0.0;

  std::cout << "prefilling signal generator " << std::endl;
  // ask for more blocks than we actually need
  SignalGenerator signal(cfg.Nfft, cfg.Nchannels, blocks+2*(cfg.Ntaps+cfg.AverageSize()), 
			 10*fundamental, cfg.sampling_rate, Ampl, noiseA);
  RefSpectrometer S(&signal,&cfg);
  SpecOutput O(&cfg);

  std::cout << "running" << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  while (S.blocks_processed()<blocks) {
    S.run(&O);
  }
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        elapsed).count();

  double rate = (S.blocks_processed()*cfg.Nfft)/(microseconds*1e-6);
  
  std::cout << "processing rate " << rate << " samples / s" << std::endl;

  return 0;
}

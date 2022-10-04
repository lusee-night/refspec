#include "RefSpectrometer.h"
#include "PowerSpecSource.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main() {
  SpecConfig cfg;

  cfg.Ntaps           = 8;
  cfg.Nchannels       = 1;
  cfg.AverageSize     = 64;
  double fundamental  = cfg.fundamental_frequency();

  size_t block_size   = cfg.Nfft;
  size_t Nblocks_gen  = cfg.AverageSize / block_size;

  size_t Nk = 10000;
  std::vector<double> kk (Nk), Pk(kk);
  double dk = 50/(Nk-1);
  double variance = 0;
  double measured_var = 0;
  for (size_t i=0;i<Nk;i++) {
    kk[i] = 50*i*dk;
    Pk[i] = exp(-pow(kk[i]-30,2)/(2*4*4)); //Pk is in dvar / Hz
    variance += Pk[i]*dk*1e6;
  }

  PowerSpecSource source(kk,Pk, cfg.sampling_rate,block_size, cfg.Nchannels,
		    Nblocks_gen, false, false);
  
  size_t count=0;
  std::vector<float> buf(block_size);
  float * pbuf = &(buf[0]);
  float ** ppbuf = &pbuf;
  while (source.data_available()) {
    source.next_block(ppbuf);
    for (size_t i=0;i<block_size;i++) {
      measured_var+=buf[i]*buf[i];
      count += 1;
    }
  }
  measured_var /= count;

  return 0;  
  SpecOutput O(&cfg);
  RefSpectrometer S(&source,&cfg);
  S.run(&O);

  for (size_t i=1;i<cfg.Nbins();i++) {
    std::cout << fundamental*i/1e6 << " " << O.avg_pspec[0][i] << std::endl; 
  }
  
  return 0;
}

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
  cfg.sampling_rate   =1.0e8;
  double fundamental  = cfg.fundamental_frequency();

  size_t block_size   = cfg.Nfft*2;
  size_t Nblocks_gen  = 100000;

  size_t Nk = 10000;
  std::vector<double> kk (Nk), Pk(kk);
  double dk = 50./(Nk-1);
  double variance = 0;
  double measured_var = 0;
  for (size_t i=0;i<Nk;i++) {
    kk[i] = i*dk;
    Pk[i] = 1e-5*exp(-pow(kk[i]-25,2)/(2*3*3)); //Pk is in dvar / Hz
    variance += Pk[i]*dk*1e6;
  }
  std::cout <<kk[9800] <<std::endl;
  PowerSpecSource source(kk,Pk, cfg.sampling_rate,block_size, cfg.Nchannels,
		    Nblocks_gen, false, false);
  
  size_t count=0;
  float **buf;
  buf = new float*[1];
  while (source.data_available()) {
    source.next_block(buf);
    for (size_t i=0;i<block_size;i++) {
      measured_var+=buf[0][i]*buf[0][i];
      count += 1;
    }
  }
  
  measured_var /= count;
  std::cout << variance << "  " <<measured_var <<" " <<sqrt(measured_var/variance)<<std::endl;
  
  return 0;  
  SpecOutput O(&cfg);
  RefSpectrometer S(&source,&cfg);
  S.run(&O);

  for (size_t i=1;i<cfg.Nbins();i++) {
    std::cout << fundamental*i/1e6 << " " << O.avg_pspec[0][i] << std::endl; 
  }
  
  return 0;
}

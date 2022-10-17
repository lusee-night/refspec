#include "RefSpectrometer.h"
#include "PowerSpecSource.h"
#include "CombSource.h"
#include "SignalCombiner.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

double PkFunc (double k) {
  return 1e-6+1e-5*exp(-pow(k-25,2)/(2*3*3)); //Pk is in dvar / Hz
}


int main() {
  SpecConfig cfg;

  cfg.Nfft            = 4096;
  cfg.Ntaps           = 8;
  cfg.Nchannels       = 1;
  cfg.AverageSize     = 80;
  cfg.notch           = true;
  double fundamental  = cfg.fundamental_frequency();

  size_t block_size   = cfg.Nfft;
  size_t Nblocks_gen  = 300000;

  CombSource PF(block_size, cfg.Nchannels, 1024,
		"data/samples/picket_fence_1024.txt", 1, 1.0);

  CombSource CalSig(block_size, cfg.Nchannels, 875,
		    "data/samples/calib_875x16.txt",16, 100.0);


  std::vector<SignalSource*> slist;
  slist.push_back(&PF);
  slist.push_back(&CalSig);
  SignalCombiner source(slist);
  
  if (false) {
    float **buf;
    buf = new float*[1];
    for (size_t j=0;j<5;j++) {
      source.next_block(buf);
      for (size_t i=0;i<block_size;i++) std::cout << buf[0][i] << std::endl; 
    }
  }

 
  // size_t Nk = 10000;
  // std::vector<double> kk (Nk), Pk(kk);
  // double dk = 70./(Nk-1);
  // double variance = 0;
  // double measured_var = 0;
  // for (size_t i=0;i<Nk;i++) {
  //   kk[i] = i*dk;
  //   Pk[i] = PkFunc(kk[i]);
  //   variance += Pk[i]*dk*1e6;
  // }

  // PowerSpecSource source(kk,Pk, cfg.sampling_rate,block_size, cfg.Nchannels,
  // 		    Nblocks_gen, false, false);
  
  SpecOutput O(&cfg);
  RefSpectrometer S(&source,&cfg);
  S.run(&O,1);
  S.run(&O,50);

  std::ofstream of("powspec.txt");

  double sw,sw2;
  sw=sw2=0.0;
  for (size_t i=1;i<cfg.Nbins();i++) {
    double k = fundamental*i/1e6;
    of << k << " " << O.avg_pspec[0][i] << " "  << std::endl; 
  }
  of.close();
  return 0;
}

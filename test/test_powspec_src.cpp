#include "RefSpectrometer.h"
#include "PowerSpecSource.h"
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

  cfg.Nfft            = 2048;
  cfg.Ntaps           = 8;
  cfg.Nchannels       = 1;
  cfg.Average1Size     = 64;
  cfg.Average2Size     = 1;
  cfg.sampling_rate   = 1.0e8;
  double fundamental  = cfg.fundamental_frequency();

  size_t block_size   = cfg.Nfft;
  size_t Nblocks_gen  = 300000;

  size_t Nk = 10000;
  std::vector<double> kk (Nk), Pk(kk);
  double dk = 70./(Nk-1);
  double variance = 0;
  double measured_var = 0;
  for (size_t i=0;i<Nk;i++) {
    kk[i] = i*dk;
    Pk[i] = PkFunc(kk[i]);
    variance += Pk[i]*dk*1e6;
  }

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
  std::cout << "Variances: "<<variance << "  " <<measured_var <<" " <<sqrt(measured_var/variance)<<std::endl;
  
  SpecOutput O(&cfg);
  RefSpectrometer S(&source,&cfg);
  S.run(&O,1);
  S.run(&O,-1);

  std::ofstream of("powspec.txt");

  double sw,sw2;
  sw=sw2=0.0;
  for (size_t i=1;i<cfg.Nbins();i++) {
    double k = fundamental*i/1e6;
    of << k << " " <<PkFunc(k) << " " << O.avg_pspec[0][i] << " "  << std::endl; 
    sw+=(O.avg_pspec[0][i]/PkFunc(k));
    sw2+=pow(O.avg_pspec[0][i]/PkFunc(k),2);
  }
  sw/=cfg.Nbins()-1;
  sw2/=cfg.Nbins()-1;
  double mean = sw;
  double err = sqrt(sw2-mean*mean)/mean;
  double errt = 1/sqrt(O.Nradiometer);
  std::cout << mean << " " << err <<" " <<errt<<std::endl;
  
    
  of.close();
  return 0;
}

#include "RefSpectrometer.h"
#include "PowerSpecSource.h"
#include "CombSource.h"
#include "WhiteNoise.h"
#include "SignalCombiner.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

#include<fftw3.h>
#include "lyra.hpp"


double PkFunc (double k) {
  return 1e-6+1e-5*exp(-pow(k-25,2)/(2*3*3)); //Pk is in dvar / Hz
}


int main(int argc, char *argv[]) {

  SpecConfig cfg;
  bool sky_signal, pf_signal, cal_signal;
  double cal_A = 1.0;

  cfg.Nfft            = 4096;
  cfg.Ntaps           = 6;
  cfg.Nchannels       = 1;
  cfg.Average1Size    = 50;
  cfg.Average2Size    = 20;
  cfg.notch           = false;
  cfg.Ncalib          = 1024;
  cfg.calib_odd       = true;
  size_t block_size = 2048;
  float **block;
  block = new float*[1];
  block[0] = new float[2048];
  
  
  auto cal_fname = std::string(argv[1]);
  auto cal_shift = atoi(argv[2]);
  auto drift = atof (argv[3]);
  std::string driftdesc = std::string(argv[4]);
  //  std::cout <<"We have : "<<driftdesc<<std::endl;;
  size_t N = driftdesc.size()+1;
  std::vector<int> steps(N);
  steps[N-1]=0.0;
  //  std::cout <<"We have : "<<N<<std::endl;
  int offset=0;
  for (int i=N-2;i>=0;i--) {
    if (driftdesc[i]=='0') {};
    if (driftdesc[i]=='+') offset++;
    if (driftdesc[i]=='-') offset--;
    steps[i]=offset;
  }

  auto CalSig = CombSource(block_size, cfg.Nchannels, 2048,
			  cal_fname,16, cal_A,0.0,drift*1e-6,0.0, cal_shift);

  std::vector<double> acc(1024);
  for (size_t i=0;i<1024;i++) acc[i]=0.0;
  //  std::cout <<"steps" << steps.size() <<std::endl;

  // 10 blocks offset (ntaps-1) 2x diff block size
  for (size_t i=0;i<10;i++) {
    CalSig.next_block(block); 
  }

  for (int step: steps) {
    // accumulate 400
    //std::cout<<step<<std::endl;
    for (size_t i=0;i<400;i++) {
      CalSig.next_block(block);
      for (size_t j=0;j<1024;j++) acc[(1024-step+j)%1024]+=block[0][j]-block[0][1024+j];
    }
  }

  std::ofstream of("driftpred.txt");
  of<<std::setprecision(16);
  for (size_t i=0;i<1024;i++) {
    of <<acc[i] <<std::endl;
  }
  of.close();
}


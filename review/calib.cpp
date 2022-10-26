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

#include "lyra.hpp"


double PkFunc (double k) {
  return 1e-6+1e-5*exp(-pow(k-25,2)/(2*3*3)); //Pk is in dvar / Hz
}


int main(int argc, char *argv[]) {
  SpecConfig cfg;
  bool sky_signal, pf_signal, cal_signal;
  double cal_A=1.0;

  size_t Ngo          = 120;

  cfg.Nfft            = 4096;
  cfg.Ntaps           = 6;
  cfg.Nchannels       = 1;
  cfg.Average1Size    = 50;
  cfg.Average2Size    = 20;
  cfg.notch           = false;
  cfg.Ncalib          = 1024;
  cfg.calib_odd       = true;





  bool help;
  auto cli = lyra::cli()
    | lyra::opt(sky_signal)
    ["-s"]["--sky"]("sky signal")
    | lyra::opt(pf_signal)
    ["-p"]["--pf"]("pf signal")
    | lyra::opt(cal_signal)
    ["-c"]["--cal"]("cal signal")
    | lyra::opt (cfg.notch)
    ["-n"]["--notch"]("spectral Notch")
    | lyra::opt (cal_A, "calibratot rescaling")
    ["-a"]["--ampl"]("Calibration amplitude rescaling")
    | lyra::opt (Ngo, "N_go")
    ["-N"]["--Ngo"]("Number of big samples")
    | lyra::help(help)
    ;

  auto result = cli.parse( { argc, argv } );
  if ( !result ) {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  if(help) {
    std::cout << cli << std::endl;
    exit(0);
  }

  if (! (sky_signal || pf_signal || cal_signal)) {
    std::cout << "No signals, assuming all." << std::endl;
    sky_signal = pf_signal = cal_signal = true;
  }

  cfg.sanity_check();

  double fundamental  = cfg.fundamental_frequency();

  size_t block_size   = cfg.Nfft;

  //WhiteNoise Noise (block_size,cfg.Nchannels,1000.0);

  std::vector<SignalSource*> slist;

  PowerSpecSource* SigNoise;
  if (sky_signal){
  SigNoise = new PowerSpecSource ("data/Pk/Pk_wnoise.txt", cfg.sampling_rate,
	   block_size, cfg.Nchannels, Ngo*cfg.AverageSize()+cfg.Ntaps);
  slist.push_back(SigNoise);
  }

  CombSource* PF;
  if (pf_signal) {
  PF = new CombSource(block_size, cfg.Nchannels, 1024,
		"data/samples/picket_fence_1024.txt", 1, 0.008);
  slist.push_back(PF);
  }

  CombSource* CalSig;
  if (cal_signal) {
  CalSig = new CombSource(block_size, cfg.Nchannels, 2048,
		    "data/samples/calib_1024x16_odd.txt",16, 1800*cal_A,0.0,0.0e-6,0.0);
  slist.push_back(CalSig);
  }

  
  SignalCombiner source(slist, true);
  
 
  SpecOutput O(&cfg);
  RefSpectrometer S(&source,&cfg);
  S.print_info();

  std::ofstream of("powspec.txt");
  std::ofstream ofc("calib.txt");
  std::ofstream ofc2("calib_meta.txt");

  for (size_t i=0;i<Ngo;i++) {
    
    S.run(&O);

    std::cout << "RMS : ";
    for (double v : source.rms()) std::cout <<v <<" ";
    std::cout << std::endl;
  

    for (size_t i=1;i<cfg.Nbins();i++) {
      double k = fundamental*i/1e6;
      of << O.avg_pspec[0][i] << " ";
    }
    of << std::endl; 

    for (size_t i=1;i<cfg.Ncalib;i++) {
      ofc <<  O.calib_out[0][i] << " ";
	}
    ofc << std::endl;

    ofc2 << O.calib_drift_count << " " <<O.calib_det <<std::endl;
  }
  
  of.close();
  ofc.close();
  ofc2.close();

  if (sky_signal) delete SigNoise;
  if (pf_signal) delete PF;
  if (cal_signal) delete CalSig;
  return 0;
}

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
  //fftw shit.
  //fftwf_init_threads();
  //fftwf_plan_with_nthreads(8);
  
  SpecConfig cfg;
  bool sky_signal, pf_signal, cal_signal;
  double cal_A = 1.0;
  double drift = 0.0;
  double ADC_range = 2;
  
  size_t Ngo          = 120;
  size_t cal_shift    = 0;
  int seed            = 100;

  cfg.Nfft            = 4096;
  cfg.Ntaps           = 6;
  cfg.Nchannels       = 1;
  cfg.Average1Size    = 16; // now set by Nnotch
  cfg.Average2Size    = 1;
  cfg.notch           = true;
  cfg.Ncalib          = 0; // 1024; now we are doing it the new way
  cfg.calib_odd       = true;

  std::string pk_fname="data/Pk/Pk_wnoise.txt";
  std::string cal_fname="data/samples/calib_filt.txt";
  std::string out_root="";
  std::string write_to="";
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
    | lyra::opt (cal_A, "calibrator rescaling")
    ["-A"]["--ampl"]("Calibration amplitude rescaling")
    | lyra::opt (Ngo, "N_go")
    ["-N"]["--Ngo"]("Number of big samples")
    | lyra::opt (cfg.Average2Size, "Average 2")
    ["--a2"]("Average 2 size")
    | lyra::opt (drift, "drift_ppm")
    ["-d"]["--drift"]("clock drift in ppm")
    | lyra::opt (pk_fname, "pk_fname")
    ["--pkfn"]("filename for power spectrum")
    | lyra::opt (cal_fname, "cal_fname")
    ["--calfn"]("filename for calibrator")
    | lyra::opt (cal_shift, "cal_shift")
    ["--calshift"]("number of records to shift calibrator by")
    | lyra::opt (out_root, "out_root")
    ["--outroot"]("output_root")
    | lyra::opt (seed, "seed")
     ["--seed"]("random seed")
    | lyra::opt (write_to, "write_to")
     ["-W"]("Do not run spectrometer, instead write to file int16 samples")
    | lyra::opt (ADC_range, "ADC_range")
     ["--ADC"]("ADC range peak to peak")
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
  SigNoise = new PowerSpecSource (pk_fname, cfg.sampling_rate,
				  block_size, cfg.Nchannels, Ngo*cfg.AverageSize()+cfg.Ntaps,
				  false, false, seed);
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
			  cal_fname,16, cal_A,0.0,drift*1e-6,0.0, cal_shift);
  slist.push_back(CalSig);
  }

  
  SignalCombiner source(slist, true);

  if (write_to!="") {
    float **buffer = new float*[cfg.Nchannels];
    size_t Nwrite = cfg.Average1Size*cfg.Average2Size*Ngo*cfg.Nfft/block_size;
    float buf;
    std::cout << "Dumping "<<Nwrite<<" blocks of "<<block_size<<" int16 samples to "<<write_to <<std::endl;
    std::ofstream of(write_to, std::ios::binary);
    int16_t minval,maxval;
    float minval_f, maxval_f;
    minval = +32767;
    maxval = -32767;
    minval_f = +1e30;
    maxval_f = -1e30;

    for (size_t i=0;i<Nwrite;i++) {
      source.next_block(buffer);
      for (size_t j=0;j<block_size;j++) {
	float val_f = buffer[0][j];
	minval_f = std::min(minval_f,val_f);
	maxval_f = std::max(maxval_f,val_f);
	int16_t val = int16_t(val_f*65536/ADC_range);
	minval = std::min(minval,val);
	maxval = std::max(maxval,val);
	of.write(reinterpret_cast<char*>(&val),sizeof(int16_t));
      }
    }
    of.close();
    std::cout << "Min val float = "<<minval_f << " Max val float = " <<maxval_f << std::endl;
    std::cout << "Min val = "<<minval << " Max val = " <<maxval << std::endl;
    std::cout << "Done. Exiting." << std::endl;
    return 0;
  }
		  
  
 
  SpecOutput O(&cfg);
  RefSpectrometer S(&source,&cfg);
  S.print_info();

  std::ofstream of(out_root+"powspec.txt");
  std::ofstream ofc(out_root+"calib.txt");
  std::ofstream ofc2(out_root+"calib_meta.txt");
  std::ofstream ofn(out_root+"notch.txt");

  of<<std::setprecision(16);
  ofc<<std::setprecision(16);
  ofc2<<std::setprecision(16);
  ofn<<std::setprecision(16);


  
  for (size_t i=0;i<Ngo;i++) {
    std::cout <<"--------" <<std::endl;
    S.run(&O);

    std::cout << "RMS : ";
    for (double v : source.rms()) std::cout <<v <<" ";
    std::cout << std::endl;
  

    for (size_t i=1;i<cfg.Nbins();i++) {
      double k = fundamental*i/1e6;
      of << O.avg_pspec[0][i] << " ";
    }
    of << std::endl; 

    if (cfg.Ncalib>0) {
      for (size_t i=0;i<cfg.Ncalib;i++) {
        ofc <<  O.calib_out[0][i] << " ";
      }
      ofc << std::endl;
    
    ofc2 << O.calib_drift_count << " " <<O.calib_det <<std::endl;
    }

    if (cfg.notch) {
      for (size_t i=1;i<cfg.Nbins();i++) {
        std::complex<float> v = O.get_notch_out(0,i);
        if (i==2) std::cout << v.real() << " " << v.imag() << std::endl;
        ofn << v.real() << " " << v.imag() << " ";
      }  
      ofn << std::endl;
    }

  }
  
  of.close();
  ofc.close();
  ofc2.close();
  ofn.close();

  if (sky_signal) delete SigNoise;
  if (pf_signal) delete PF;
  if (cal_signal) delete CalSig;
  return 0;
}

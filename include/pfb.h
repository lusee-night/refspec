#pragma once

#include<fftw3.h>

enum window_t { None, Hanning, Hamming };

class PolyphaseFilterBank {

 public:

  // constructor using Nfft point FFT, Ntaps and a window. Sampling rate on enters
  // for frequency calculation
  PolyphaseFilterBank (double sampling_rate, int Nfft, int Ntaps, window_t window);

  // Destructor
  ~PolyphaseFilterBank();
  
  // must call once to set up fftw plan
  void setup_plan (fftwf_complex *data_out_example);
  // process filter for a vector [Ntaps][Nfft] sized into [Nfft//2+1] output
  void process (float **data_in, fftwf_complex *data_out);

  int get_Ncomplex() { return Ncomplex; }
  
 private:

  double sampling_rate; 
  int Nfft, Ntaps, Ncomplex;
  window_t window;
  float **weights;
  float *work;
  fftwf_plan plan;
  bool have_plan;
  int align_out;
};


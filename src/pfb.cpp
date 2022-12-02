#include "pfb.h"
#include <assert.h>
#include <math.h>
#include <iostream>

PolyphaseFilterBank::PolyphaseFilterBank (double sampling_rate, int Nfft, int Ntaps, window_t window) :
  sampling_rate(sampling_rate),Nfft(Nfft), Ntaps(Ntaps), window(window)

{
  // nothing else implemented at the moment
  //assert ( window == None);
  assert (Nfft % 2 == 0);
  
  Ncomplex = Nfft / 2 + 1;
  weights = new float*[Ntaps];
  work = fftwf_alloc_real (Nfft);

  for (int i=0; i<Ntaps; i++) {weights[i] = fftwf_alloc_real (Nfft);}

  // Now need to set up the sinc weight
  double L = Ntaps * Nfft;
  double sw=0.0;
  for (int i=0; i<Ntaps; i++) {
    for (int j=0; j<Nfft; j++) {
      // replaces this with a proper PI
      double x = M_PI*(i*Nfft+j - L/2)/Nfft;
      if (x==0)
	      weights [i][j] = 1;
      else
	      weights [i][j] = sin(x)/x;
      
      double xw = 2 * M_PI * (i*Nfft+j) / (L - 1);

      if (window == Hamming)
	      weights[i][j] *= 0.53836 - 0.46164 * cos(xw);
      if (window == Hanning)
	      weights[i][j] *= 0.5 * (1 - cos(xw));
      if (window == BlackmanNuttall)
	      weights[i][j] *= 0.3635819 - 0.4891775 * cos(xw) + 0.1365995 * cos(2*xw) - 0.01064118 * cos(3*xw); 

      sw += pow(weights[i][j],1);
    }
  }

  for (int i=0; i<Ntaps; i++) {for (int j=0; j<Nfft; j++) {weights[i][j]/=sw;}}

  have_plan = false;
}

PolyphaseFilterBank::~PolyphaseFilterBank () {
  if (have_plan)
    fftwf_destroy_plan(plan);
  fftwf_free (work);
  for (int i=0; i<Ntaps; i++) fftwf_free(weights[i]);
  delete weights;
}


void PolyphaseFilterBank::setup_plan (fftwf_complex *data_out_example) {
    align_out = fftwf_alignment_of((float*)data_out_example);
    plan = fftwf_plan_dft_r2c_1d(Nfft, work, data_out_example, FFTW_MEASURE || FFTW_DESTROY_INPUT);
    assert (plan != NULL);
    have_plan = true;


}



void PolyphaseFilterBank::process (float **data_in, fftwf_complex *data_out) {
  for (int j=0;j<Nfft;j++) work[j] = 0.0;
  for (int i=0; i<Ntaps; i++) {for (int j=0; j<Nfft; j++) {work[j] += data_in[i][j]*weights[i][j];}}

  assert(have_plan);
  assert(fftwf_alignment_of((float*)data_out) == align_out);
  fftwf_execute_dft_r2c(plan, work, data_out);
}

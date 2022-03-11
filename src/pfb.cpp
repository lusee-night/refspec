#include "pfb.h"
#include <assert.h>
#include <math.h>

PolyphaseFilterBank::PolyphaseFilterBank (double sampling_rate, int Nfft, int Ntaps, window_t window) :
  sampling_rate(sampling_rate),Nfft(Nfft), Ntaps(Ntaps), window(window)

{
  // nothing else implemented at the moment
  assert ( window == None);
  assert (Nfft % 2 == 0);
  Ncomplex = Nfft / 2 + 1;
  weights = new float*[Ntaps];
  work = fftwf_alloc_real (Nfft);
  for (size_t i=0; i<Ntaps; i++) {
    weights[i] = fftwf_alloc_real (Nfft);
  }
  // Now need to set up the sinc weight
  double L = Ntaps * Nfft;
  for (size_t i=0; i<Ntaps; i++) {
    for (size_t j=0; j<Nfft; j++) {
      // replaces this with a proper PI
      double x = M_PI*(i*Nfft+j - L/2)/Nfft; 
      if (x==0)
	weights [i][j] = 1;
      else
	weights [i][j] = sin(x)/x;
      // add window function
    }
  }
  have_plan = false;
}

PolyphaseFilterBank::~PolyphaseFilterBank () {
  if (have_plan)
    fftwf_destroy_plan(plan);
}


void PolyphaseFilterBank::setup_plan (fftwf_complex *data_out_example) {
    align_out = fftwf_alignment_of((float*)data_out_example);
    plan = fftwf_plan_dft_r2c_1d(Nfft, work, data_out_example, FFTW_MEASURE || FFTW_DESTROY_INPUT);
    assert (plan != NULL);
    have_plan = true;
      

}



void PolyphaseFilterBank::process (float **data_in, fftwf_complex *data_out) {
  for (size_t j=0;j<Nfft;j++) work[j] = 0.0;

  for (size_t i=0; i<Ntaps; i++) {
    for (size_t j=0; j<Nfft; j++) {
      work[j] += data_in[i][j]*weights[i][j];
    }
  }
  assert(have_plan);
  assert(fftwf_alignment_of((float*)data_out) == align_out);
  fftwf_execute_dft_r2c(plan, work, data_out);
}

#include "pfb.h"
#include <math.h>
#include <iostream>


#define SAMPLING_RATE (1e8)
#define NFFT (2048)
#define NTAPS (16)

int main() {
  PolyphaseFilterBank P(SAMPLING_RATE, NFFT, NTAPS, None);
  float ** data;
  data = new float*[NTAPS];
  for (size_t j=0;j<NTAPS;j++) data[j] = fftwf_alloc_real(NFFT);
  fftwf_complex *data_out = fftwf_alloc_complex(NFFT/2+1);
  P.setup_plan(data_out);


  for (double freq = 3; freq<7; freq+=0.01) {
    for (size_t j=0;j<NTAPS;j++) {
      for (size_t i=0;i<NFFT;i++) {
	float x = j*NFFT+i;
	data[j][i] = cos(x*freq*(2*M_PI)/NFFT);
      }
    }
    P.process(data, data_out);
    std::cout << freq << " " << (data_out[5][0]*data_out[5][0] + data_out[5][1]*data_out[5][1])  <<std::endl;
  }
  return 0;
}

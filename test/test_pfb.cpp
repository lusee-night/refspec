#include "pfb.h"
#include <math.h>

#define SAMPLING_RATE (1e8)
#define NFFT (2048)
#define NTAPS (4)

int main() {
  PolyphaseFilterBank P(SAMPLING_RATE, NFFT, NTAPS, None);
  float ** data;
  data = new float*[NTAPS];
  for (size_t j=0;j<NTAPS;j++) {
    data[j] = fftwf_alloc_real(NFFT);
    for (size_t i=0;i<NFFT;i++) {
      float x = j*NFFT+i;
      data[j][i] = sin(x*0.01);     
    }
  }

  fftwf_complex *data_out = fftwf_alloc_complex(NFFT/2+1);
  P.setup_plan(data_out);
  P.process(data, data_out);
  return 0;
}

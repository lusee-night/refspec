#include "testing.h"
#include <cassert>

Testing::Testing():  Nchannels(1), sampling_rate(1.024e8), Nfft(4096),
			    Ntaps(8), window(None), Average1Size(80), Average2Size(10), Ncalib(0),
			    calib_odd (false), calib_subint(5),
			    notch(false), zoomin_st(0), zoomin_en(0)
{
  for (size_t i=0;i<MAX_CHANNELS;i++) {
    plus_channel[i] = i;
    minus_channel[i] = -1;
  }
}

void Testing::sanity_check() const {
  size_t zoomin_fact=0;//d/ zoom_weights.size();
  if (Ncalib>0) {
    // numerology of death
    assert(calib_subint == 5);
    assert( (AverageSize() * Nfft) % (calib_subint * Ncalib * (1+(calib_odd))) == 0);
  }
}



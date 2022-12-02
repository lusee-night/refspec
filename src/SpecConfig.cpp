#include "SpecConfig.h"
#include <cassert>

SpecConfig::SpecConfig():  Nchannels(1), sampling_rate(1.024e8), Nfft(4096),
			    Ntaps(8), window(None), Average1Size(80), Average2Size(10), Ncalib(0),
			    calib_odd (false), calib_subint(5),
			    notch(false), zoomin_st(0), zoomin_en(0),zoomin_fact(0)
{
  for (size_t i=0;i<MAX_CHANNELS;i++) {
    plus_channel[i] = i;
    minus_channel[i] = -1;
  }
}

void SpecConfig::sanity_check() const {
  if (zoomin_fact>0) {
    assert(Average1Size % zoomin_fact==0);
    assert((zoomin_fact==0) || (zoomin_fact==3) || (zoomin_fact==4)|| (zoomin_fact==5));
  }
  if (Ncalib>0) {
    // numerology of death
    assert(calib_subint == 5);
    assert( (AverageSize() * Nfft) % (calib_subint * Ncalib * (1+(calib_odd))) == 0);
  }
}



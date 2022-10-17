#include "SpecConfig.h"
#include <cassert>

SpecConfig::SpecConfig() :  Nchannels(1), sampling_rate(1.024e8), Nfft(4096),
			    Ntaps(8), window(None), AverageSize(64), Ncalib(0), notch(false),
			    zoomin_st(0), zoomin_en(0),zoomin_fact(0)
{
  for (size_t i=0;i<MAX_CHANNELS;i++) {
    plus_channel[i] = i;
    minus_channel[i] = -1;
  }
  if (zoomin_fact>0) {
    assert(AverageSize%zoomin_fact==0);
    assert((zoomin_fact==0) || (zoomin_fact==3) || (zoomin_fact==4)|| (zoomin_fact==5));
  }
}

#include "SpecConfig.h"

SpecConfig::SpecConfig() :
  mode (production), Nchannels(1), sampling_rate(1e8), Nfft(4096),
  Ntaps(8), window(None), average_samples(64), Ncalib(0)
{
  plus_channel[0] = 0;
  minus_channel[0] = -1;
}

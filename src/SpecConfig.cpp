#include "SpecConfig.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

SpecConfig::SpecConfig() :  mode (production), Nchannels(1), sampling_rate(1e8), Nfft(4096),
			    Ntaps(8), window(None), AverageSize(64), Ncalib(0), notch(false)
{
  for (size_t i=0;i<MAX_CHANNELS;i++) {
    plus_channel[i] = i;
    minus_channel[i] = -1;
 }

}

#pragma once

#include "SpecConfig.h"

class RefSpectrometer{


 public:
  RefSpectrometer (SingnalSource *source, SignalSink *sink, SpecConfig config);

  // run for nblock, forever if 0
  void run (int nblocks);


}
  

			 

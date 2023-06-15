#include "SpecConfig.h"
#include <cassert>

SpecConfig::SpecConfig():  Nchannels(1), sampling_rate(1.024e8), Nfft(4096),
			    Ntaps(8), window(None), Average1Size(80), Average2Size(10), Ncalib(0),
			    calib_odd (false), calib_subint(5),
			    notch(false), zoomin_st(0), zoomin_en(0) {

    for (size_t i=0;i<MAX_CHANNELS;i++) {
      plus_channel[i]   = i;
      minus_channel[i]  = -1;
  }
}
// ---
void SpecConfig::sanity_check() const {
  size_t zoomin_fact= zoom_weights.size();
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
// ---
void SpecConfig::resize_zoom(size_t  Nzoom, size_t Nweights) {
  zoom_weights.resize(Nzoom);
  zoom_weights_imag.resize(Nzoom);
  for (size_t i=0;i<Nzoom;i++) {
    zoom_weights[i].resize(Nweights); 
    zoom_weights_imag[i].resize(Nweights);
  }
}
// ---
void SpecConfig::set_zoom_weight (size_t i, size_t j, float value_real, float value_imag) {
  assert(i<zoom_weights.size());
  assert(j<zoom_weights[i].size());
  zoom_weights[i][j] = value_real;
  zoom_weights_imag[i][j] = value_imag;
}

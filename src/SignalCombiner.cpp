#include "SignalCombiner.h"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

SignalCombiner::SignalCombiner(bool get_rms): calc_rms(get_rms), Nchannels(0) {};

SignalCombiner::SignalCombiner(std::vector<SignalSource*> sources, bool get_rms) : sources(sources),
										   calc_rms(get_rms)
{
  Nsources = sources.size();
  assert(Nsources>0);
  
  block_size = sources[0]->get_block_size();
  Nchannels = sources[0]->get_Nchannels();
  for (SignalSource* source : sources) {
    assert(source->get_block_size() == block_size);
    assert(source->get_Nchannels() == Nchannels);
  }

  buf = new float*[Nchannels];
  inbuf = new float*[Nchannels];
  for (size_t i=0;i<Nchannels;i++) buf[i] = fftwf_alloc_real (block_size);
  if (calc_rms) {
    var.clear();
    var.resize(Nsources,0.0);
    rms_counter = 0;
  }
}


SignalCombiner::SignalCombiner::~SignalCombiner() {
  if(Nchannels>0) {
    for (size_t i=0;i<Nchannels;i++) fftwf_free(buf[i]);
    delete buf;
  }
}

// ---
// This is the original next_block,
// and below is the version designed to work with the internal buffer
// ---
void SignalCombiner::SignalCombiner::next_block(float **place) {
  bool first_block = true;
  size_t i=0;
  for (SignalSource* source : sources) {
    source->next_block(inbuf);
    for (size_t j=0;j<Nchannels;j++) {
      for (size_t k=0;k<block_size;k++) {
	if (first_block) buf[j][k] = inbuf[j][k]; else buf[j][k] += inbuf[j][k];
	if (calc_rms) {
	  var[i] += pow(inbuf[j][k],2); 
	  rms_counter += 1; // note we're overcounting here
	}
      }
    }
    first_block=false;
    i++;
  }
  for (size_t i=0;i<Nchannels;i++) place[i] = buf[i];
}

void SignalCombiner::next_block() {
  next_block(internal);
}
// ---

std::vector<double> SignalCombiner::rms() const {
  if (!calc_rms) return std::vector<double>(0);
  std::vector<double> toret (Nsources);
  for (size_t i=0;i<Nsources; i++) {
    toret[i] = sqrt(var[i]/(rms_counter/Nsources));
  }
  return toret;
}

bool SignalCombiner::data_available() const {
  bool avail = true;
  for (SignalSource* source : sources) avail = (avail && source->data_available());
  return avail;
}

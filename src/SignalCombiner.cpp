#include "SignalCombiner.h"
#include <cassert>

SignalCombiner::SignalCombiner(std::vector<SignalSource*> sources) : sources(sources)
{
  size_t Nsources = sources.size();
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
}


SignalCombiner::SignalCombiner::~SignalCombiner() {
  for (size_t i=0;i<Nchannels;i++) fftwf_free(buf[i]);
  delete buf;
}

void SignalCombiner::SignalCombiner::next_block(float **place) {
  bool first_block = true;
  for (SignalSource* source : sources) {
    source->next_block(inbuf);
    for (size_t j=0;j<Nchannels;j++) {
      for (size_t k=0;k<block_size;k++) {
	if (first_block) buf[j][k] = inbuf[j][k]; else buf[j][k] += inbuf[j][k];
      }
    }
    first_block=false;
  }
  for (size_t i=0;i<Nchannels;i++) place[i] = buf[i];
}

bool SignalCombiner::data_available() const {
  bool avail = true;
  for (SignalSource* source : sources) avail = (avail && source->data_available());
  return avail;
}

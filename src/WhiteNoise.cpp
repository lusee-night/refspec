#include <math.h>
#include <assert.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "WhiteNoise.h"
#include "pfb.h"

WhiteNoise::WhiteNoise (size_t block_size, size_t Nchannels, float rms) :
  SignalSource(block_size, Nchannels)
{
  buffer = fftwf_alloc_real (block_size);
  gauss = std::normal_distribution<double>(0.0,rms);

}

WhiteNoise::~WhiteNoise() {
  fftwf_free(buffer);
}



void WhiteNoise::next_block(float **place) {
  for (size_t i=0;i<block_size;i++) buffer[i] = gauss(generator);
  for (size_t i=0;i<Nchannels;i++) place[i]=buffer;
}
  

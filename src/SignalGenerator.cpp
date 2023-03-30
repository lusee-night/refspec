#include <math.h>
#include <assert.h>

#include "SignalGenerator.h"
#include "pfb.h"

SignalGenerator::SignalGenerator (size_t block_size, size_t Nchannels, size_t Nblocks, float frequency,
				  float sampling_rate, float Ampl, float noise_rms) :
  SignalSource(block_size, Nchannels), Nblocks(Nblocks), frequency(frequency), sampling_rate(sampling_rate),
  Ampl(Ampl), noise_rms(noise_rms)
{
  buffer = fftwf_alloc_real (block_size * Nblocks);
  cur_block = 0;
  phase_offset = 0;
  fill_buffer();
}

SignalGenerator::~SignalGenerator() {
  fftwf_free(buffer);
}



void SignalGenerator::next_block(float **place) {
  float *cur = &(buffer[cur_block * block_size]);
  if ((cur_block == 0) && (phase_offset > 0)) fill_buffer();
  cur_block = (cur_block+1) % Nblocks;;
  for (size_t i=0;i<Nchannels;i++) place[i]=cur;
}
  
void SignalGenerator::next_block() {
}

void SignalGenerator::fill_buffer() {
  size_t N = block_size * Nblocks;
  for (size_t i=0;i<N;i++) buffer[i]=0;
  double omega_i = frequency/sampling_rate*2*M_PI;
  
  if (Ampl > 0) {
    for (size_t i=0;i<N;i++) 
      buffer[i] += Ampl * cos(phase_offset + omega_i * i);
  };

  if (noise_rms > 0 ) {
    assert(false); // implement
  }

  phase_offset += omega_i * N;
}

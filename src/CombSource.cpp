#include <math.h>
#include <assert.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "CombSource.h"
#include "pfb.h"

CombSource::CombSource (size_t block_size, size_t Nchannels,
			size_t Nsamples,  std::string filename,
			size_t oversample,
			float A0, float beta_A,
			float alpha_t, float beta_t, size_t sample_shift, bool zero_mean ):
  SignalSource(block_size, Nchannels), Nsamples(Nsamples), oversample(oversample),
  A0(A0), beta_A(beta_A), alpha_t(alpha_t), beta_t(beta_t), cc(0),
  simple_copy ((alpha_t==0) && (beta_t==0) && (oversample==1))
{
  Nsamples_raw = Nsamples * oversample;
  buffer = fftwf_alloc_real (block_size);
  buffer_raw = fftwf_alloc_real (Nsamples_raw);

  std::ifstream inf(filename, std::ios::in);
  if (!inf.is_open()) {
    std::cout << "Cannot open "<<filename<<std::endl;
    exit(1);
  }
  for (size_t i=0;i<Nsamples_raw;i++) {
    inf >> buffer_raw[(sample_shift+i)%Nsamples_raw];
  }

  if (zero_mean) {
    float mean = 0;
    for (size_t i=0;i<Nsamples_raw;i++) mean += buffer_raw[i];
    mean /= Nsamples_raw;
    for (size_t i=0;i<Nsamples_raw;i++) buffer_raw[i] -= mean;
  }

  inf.close();
}

CombSource::~CombSource() {
  fftwf_free(buffer);
  fftwf_free(buffer_raw);
}



void CombSource::next_block(float **place) {
  size_t offset = cc % Nsamples;
  if (simple_copy) {
    assert(offset==0);
    for (size_t i=0;i<block_size;i++) {
      float A = A0*(1+beta_A * cc);
      buffer[i] = buffer_raw[(offset+i)%Nsamples]*A;
      cc++;
    }
  } else {
    for (size_t i=0;i<block_size;i++) {
      //double ti = cc*(alpha_t + beta_t * cc); // unstable
      double ti = offset+i+cc*(alpha_t + beta_t * cc);
      ti -= Nsamples*int(ti/Nsamples);
      while (ti<0) ti+=Nsamples;
      float A = A0*(1+beta_t * cc);
      size_t ndx = int(ti*oversample);
      buffer[i] = buffer_raw[ndx]*A;
      cc++;
    }
  }
  for (size_t i=0;i<Nchannels;i++) place[i]=buffer;
}
  
void CombSource::next_block() {
  next_block(internal);
}
  
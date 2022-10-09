#include <math.h>
#include <assert.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "FileStreamSource.h"
#include "pfb.h"

FileStreamSource::FileStreamSource (size_t block_size, size_t Nchannels, std::string filename) :
  SignalSource(block_size, Nchannels), cur_block(0)
{
  std::filesystem::path p(filename);
  auto size = file_size(p);
  Nblocks = int (size/block_size/2); // two bytes per block
  size_t N = block_size * Nblocks;
  buffer = fftwf_alloc_real (N);
  int16_t *ibuffer = new int16_t[N];
  
  std::ifstream inf(filename, std::ios::in | std::ios::binary);
  if (!inf.is_open()) {
    std::cout << "Cannot open "<<filename<<std::endl;
    exit(1);
  }
  inf.read(reinterpret_cast<char*>(ibuffer), N*sizeof(int16_t));
  std::cout << "Read "<<N<<" records from "<<filename<<std::endl;
  for (size_t i=0;i<N;i++) buffer[i] = float(ibuffer[i]);
  inf.close();
  delete ibuffer;
}

FileStreamSource::~FileStreamSource() {
  fftwf_free(buffer);
}



void FileStreamSource::next_block(float **place) {
  float *cur = &(buffer[cur_block * block_size]);
  cur_block = (cur_block+1) % Nblocks;;
  for (size_t i=0;i<Nchannels;i++) place[i]=cur;
}
  

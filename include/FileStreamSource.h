#pragma once

#include "SignalSource.h"
#include <cstdint>
#include <stddef.h>
#include <string>

class FileStreamSource: public SignalSource {

public:
  FileStreamSource(size_t block_size, size_t Nchannels, std::string filename);

  ~FileStreamSource();
  
  virtual void next_block(float **place); 

private:
  float *buffer;
  
  size_t cur_block, Nblocks ;
};

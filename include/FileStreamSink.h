
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#include "SignalSource.h"


class FileStreamSink {

public:
  
  FileStreamSink(SignalSource* source, std::vector<std::string> filenames, float bit_level = 1.0, size_t bits=14);

  FileStreamSink(SignalSource* source, std::string filename, float bit_level = 1.0, size_t bits=14):
    FileStreamSink(source, std::vector<std::string>(1,filename), bit_level, bits) {}

    float min_float() {return minval_f;}
    float max_float() {return maxval_f;}
    float variance() {return var;}
    float rms() {return std::sqrt(var);}
    int16_t min_int() {return minval;}
    int16_t max_int() {return maxval;}


    ~FileStreamSink();
  void write_data (size_t Nsamples); 

private:
    SignalSource* source;
    size_t Nchannels, block_size, bits;
    float bit_level;
    std::vector<std::ofstream> files;
    float minval_f, maxval_f;
    float var;
    int minval, maxval;
};
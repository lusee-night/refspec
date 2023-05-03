#include "FileStreamSink.h"
#include <iostream>
#include <assert.h>
#include <math.h>

FileStreamSink::FileStreamSink(SignalSource* source, std::vector<std::string> filenames, float bit_level, size_t bits) : 
    source(source), bit_level(bit_level), bits(bits)
 {
    assert (bits<=16);
    Nchannels = source->get_Nchannels();
    assert (Nchannels <= MAX_CHANNELS);
    block_size = source->get_block_size();
    assert(filenames.size() == Nchannels);
    for (auto filename : filenames) {
        files.push_back(std::ofstream(filename, std::ios::out | std::ios::binary));
    }
}


void FileStreamSink::write_data (size_t Nsamples) {
    size_t Nblocks = Nsamples / block_size;
    minval_f = 1e100;
    maxval_f = -1e100;
    minval = 32767;
    maxval = -32767;
    var = 0.0;
    float *buffer[MAX_CHANNELS];
    for (int j=0; j<Nblocks; j++) {
        source->next_block(buffer);
        for (int k=0; k<block_size; k++) {
            for (int c=0; c<Nchannels; c++) {
                float val_f = buffer[c][k];
                var+=val_f*val_f;
	            minval_f = std::min(minval_f,val_f);
	            maxval_f = std::max(maxval_f,val_f);
	            int val_i = std::round(val_f/bit_level);
	            minval = std::min(minval,val_i);
	            maxval = std::max(maxval,val_i);
                int16_t val_16 = int16_t(val_i);
	            files[c].write(reinterpret_cast<char*>(&val_16),sizeof(int16_t));
            }
        }
    }
    var/=Nblocks*block_size;
    int bit_range = 1<<(bits-1); // one bit is for sign
    if ((minval < -bit_range) || (maxval > bit_range-1)) {
        std::cout << "Warning: bit_overflow" << std::endl;
    }
}

FileStreamSink::~FileStreamSink() {
}
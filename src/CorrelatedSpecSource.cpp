#include "CorrelatedSpecSource.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <fftw3.h>
#include <random>
#include <assert.h>
#include <omp.h>

CorrelatedSpecSource::CorrelatedSpecSource(const std::vector<double> &kk,
		  const CholKMatrix &C,
		  float sampling_rate,
		  size_t block_size, size_t Nchannels,
			size_t Nblocks_gen, bool repeat, bool second_fourier, int seed, bool verb):
  SignalSource(block_size, Nchannels, verb), sampling_rate(sampling_rate), cur_block(0), Nblocks(Nblocks_gen),
  repeat(repeat), second_fourier(second_fourier), buffer(NULL) {
  // first some sanity checks
  assert(kk.size()==C.size());
  assert(C[0].size == Nchannels);
  assert(C[0][0].size == Nchannels);
  generate_data(kk,Pk,seed);
};

CorrelatedSpecSource::CorrelatedSpecSource(std::string filename, float sampling_rate,
				 size_t block_size, size_t Nchannels,
				 size_t Nblocks_gen, bool repeat, bool second_fourier, int seed, bool verb) :
  SignalSource(block_size, Nchannels, verb),  sampling_rate(sampling_rate),  cur_block(0), Nblocks(Nblocks_gen),
  repeat(repeat), second_fourier(second_fourier), buffer(NULL) 
{
  std::ifstream inf(filename, std::ios::in);
  if (!inf.is_open()) {
    std::cout << "Cannot open "<<filename<<std::endl;
    return;
  }

  throw std::runtime_error("Not implemented");
  std::vector<double> kk;  
  while (!inf.eof()){
    double kk_, Pk_;
    inf >> kk_ >> Pk_;
    kk.push_back(kk_);
    Pk.push_back(Pk_);
  }
  inf.close();
  generate_data(kk,Pk,seed);
};



CorrelatedSpecSource::~CorrelatedSpecSource() {
  if (buffer)  fftwf_free(buffer);
}


void CorrelatedSpecSource::generate_data(const std::vector<double> &kk, const CholKMatrix &C, int seed) {
    size_t N = Nblocks*block_size;
    size_t Nfft = N/2+1;
    size_t Nc = C[0].size();
    std::vector<fftwf_complex*> fourier(Nc)
    for (size_t i=0; i<Nc; i++) {
        fourier[i]=fftwf_alloc_complex(Nfft);
        buffer[i] = fftwf_alloc_real(N);
    }
    bool align_out = fftwf_alignment_of((float*)buffer[0]);

    if(verbose) { 
        std::cout << "*** Planning " << N << " samples = " << N/sampling_rate << " seconds. ***"  <<std::endl;
    }

    fftwf_set_timelimit(1.0);
    fftwf_plan plan = fftwf_plan_dft_c2r_1d(N, fourier[0], buffer[0], FFTW_DESTROY_INPUT || FFTW_ESTIMATE);
    if(verbose) {
        std::cout << "*** Generating... ***" <<std::endl;
    }
    // now generate fourier variates
    size_t ki=0;
    size_t kj=1;
    float domega = sampling_rate/N/1e6; // we work in MHz

    for (auto &f: fourier) f[0][0] = f[0][1]=0; // DC component

    //std::vector<std::default_random_engine> generator(0);;
    //std::vector<std::normal_distribution<double>> gauss(0);
    // size_t Nthreads= omp_get_num_threads();
    // for (size_t i=0;i<Nthreads;i++) {
    //   generator.push_back(std::default_random_engine());
    //   gauss.push_back(std::normal_distribution<double>(0.0,1.0));
    // }

    std::default_random_engine generator;
    generator.seed(seed);
    if(verbose) {
        std::cout << "*** Seed: " << seed << " ***" << std::endl;
    }
    std::normal_distribution<double> gauss(0.0,1.0);

    size_t NUp = N/2 + (second_fourier*N/2);

    double Pnorm = sqrt(sampling_rate/N/2);
    std::vector< std::complex<float> > ran(Nchannels);    
    //#pragma omp parallel for shared(fourier,kk,Pk, Nfft,Pnorm,NUp,domega, gauss, generator) firstprivate(ki,kj)
    for (size_t i=1;i<=NUp;i++) {
        //size_t mythread = omp_get_thread_num();
        float omega = domega*i;
        size_t j;
        std::vector<std::vector<complex>> Chere;
        bool dohere = true;
        if ((omega<=kk[0])||(omega>kk[kk.size()-1])) {
        dohere = false;
        } else {
        while (omega>kk[kj]) {ki++; kj++;}
        // now interpolate / extrapolate
        Chere = (C[ki]+(C[kj]-C[ki])/(kk[kj]-kk[ki])*(omega-kk[ki]); // in dvar/Hz
        }

        if (i<=Nfft) j=i; else j = N-i;
        
        for (complex& c: ran) {c=(gauss(generator)+1i*gauss(generator))/sqrt(2);}  }
        for (int k=0; k<Nchannels; k++) {
            std::complex<float> out=0 + 1i*0;
            for (int l=0; l<Nchannels; l++)
                out[k]+=Chere[k][l]*ran[l];
            fourier[k][j][0]=out.real();
            fourier[k][j][1]=out.imag();
        }
    }

    if(verbose) {
        std::cout << "*** Transforming... ***" <<std::endl;
    }
    for (int k=0; k<Nchannels; k++){
        std::cout << "  Channel "<<k<<"..." <<std::endl;
        fftwf_execute_df2_c2r(plan, fourier[k], buffer[k]);
        fftwf_free(fourier[k]);
    }    
    fftwf_destroy_plan(plan);
    if(verbose) {
        std::cout << "*** Done ***" <<std::endl;
}

}


bool CorrelatedSpecSource::data_available() const
  {
    return repeat || cur_block<(Nblocks-1);
    
  }

void CorrelatedSpecSource::next_block(float **place) {
  for (size_t i=0;i<Nchannels;i++) place[i]=&(buffer[i][cur_block * block_size]);;
  cur_block = (cur_block+1) % Nblocks;
}

void CorrelatedSpecSource::next_block() {
  next_block(internal);
}

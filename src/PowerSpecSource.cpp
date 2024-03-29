#include "PowerSpecSource.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <fftw3.h>
#include <random>
#include <assert.h>
#include <omp.h>

PowerSpecSource::PowerSpecSource(const std::vector<double> &kk,
		  const std::vector<double> &Pk,
		  float sampling_rate,
		  size_t block_size, size_t Nchannels,
			size_t Nblocks_gen, bool repeat, bool second_fourier, int seed, bool verb):
  SignalSource(block_size, Nchannels, verb), sampling_rate(sampling_rate), cur_block(0), Nblocks(Nblocks_gen),
  repeat(repeat), second_fourier(second_fourier), buffer(NULL) {
  generate_data(kk,Pk,seed);
};

PowerSpecSource::PowerSpecSource(std::string filename, float sampling_rate,
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

  std::vector<double> kk,Pk;
  while (!inf.eof()){
    double kk_, Pk_;
    inf >> kk_ >> Pk_;
    kk.push_back(kk_);
    Pk.push_back(Pk_);
  }
  inf.close();
  generate_data(kk,Pk,seed);
};



PowerSpecSource::~PowerSpecSource() {
  if (buffer)  fftwf_free(buffer);
}


void PowerSpecSource::generate_data(const std::vector<double> &kk, const std::vector<double> &Pk, int seed) {
  size_t N = Nblocks*block_size;
  size_t Nfft = N/2+1;
  fftwf_complex* fourier = fftwf_alloc_complex(Nfft);
  buffer = fftwf_alloc_real(N);
  bool align_out = fftwf_alignment_of((float*)buffer);


  if(verbose) { 
    std::cout << "*** Planning " << N << " samples = " << N/sampling_rate << " seconds. ***"  <<std::endl;
  }

  fftwf_set_timelimit(1.0);
  fftwf_plan plan = fftwf_plan_dft_c2r_1d(N, fourier, buffer, FFTW_DESTROY_INPUT || FFTW_ESTIMATE);
  if(verbose) {
    std::cout << "*** Generating... ***" <<std::endl;
  }
  // now generate fourier variates
  size_t ki=0;
  size_t kj=1;
  float domega = sampling_rate/N/1e6; // we work in MHz
  
  fourier[0][0] = fourier[0][1]=0; // DC component
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

  //#pragma omp parallel for shared(fourier,kk,Pk, Nfft,Pnorm,NUp,domega, gauss, generator) firstprivate(ki,kj)
  for (size_t i=1;i<=NUp;i++) {
    //size_t mythread = omp_get_thread_num();
    float omega = domega*i;
    size_t j;
    double Phere;
    if ((omega<=kk[0])||(omega>kk[kk.size()-1])) {
      Phere = 0.0;
     } else {
      while (omega>kk[kj]) {ki++; kj++;}
      // now interpolate / extrapolate
      Phere = Pk[ki]+(Pk[kj]-Pk[ki])/(kk[kj]-kk[ki])*(omega-kk[ki]); // in dvar/Hz
     };

    double A = sqrt(Phere/2)*Pnorm; // fix norm factors
    if (std::isnan(A)) {
	    std::cout << "Fatal, negative power spec <<"<<omega<<" " <<Phere <<std::endl;
	    exit(1);
      }

    if (i<=Nfft) j=i; else j = N-i;
    //fourier[j][0]=A*gauss[mythread](generator[mythread]);
    //fourier[j][1]=A*gauss[mythread](generator[mythread]);
    fourier[j][0]=A*gauss(generator);
    fourier[j][1]=A*gauss(generator);
  }
  
  if(verbose) {
    std::cout << "*** Transforming... ***" <<std::endl;
  }

  fftwf_execute(plan);
  fftwf_free(fourier);
  fftwf_destroy_plan(plan);
  
  // std::cout << "*** V ***" << get_verbose() << std::endl;
  
  if(verbose) {
    std::cout << "*** Done ***" <<std::endl;
  }

}


bool PowerSpecSource::data_available() const
  {
    return repeat || cur_block<(Nblocks-1);
    
  }

void PowerSpecSource::next_block(float **place) {
  float *cur = &(buffer[cur_block * block_size]);
  cur_block = (cur_block+1) % Nblocks;
  for (size_t i=0;i<Nchannels;i++) place[i]=cur;
}

void PowerSpecSource::next_block() {
  next_block(internal);
}

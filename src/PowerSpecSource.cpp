#include "PowerSpecSource.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <fftw3.h>
#include <random>

PowerSpecSource::PowerSpecSource(const std::vector<double> &kk,
		  const std::vector<double> &Pk,
		  float sampling_rate,
		  size_t block_size, size_t Nchannels,
		  size_t Nblocks_gen, bool repeat, bool second_fourier):
  SignalSource(block_size, Nchannels), sampling_rate(sampling_rate), cur_block(0), Nblocks(Nblocks_gen), repeat(repeat),
  second_fourier(second_fourier) {
  generate_data(kk,Pk);
};

PowerSpecSource::PowerSpecSource(std::string filename, float sampling_rate,
				 size_t block_size, size_t Nchannels,
				 size_t Nblocks_gen, bool repeat, bool second_fourier) :
  SignalSource(block_size, Nchannels),  sampling_rate(sampling_rate),  cur_block(0), Nblocks(Nblocks_gen), repeat(repeat),
  second_fourier(second_fourier) 
{
  std::ifstream inf(filename, std::ios::in);
  if (!inf.is_open()) {
    std::cout << "Cannot open "<<filename<<std::endl;
    exit(1);
  }

  std::vector<double> kk,Pk;
  while (!inf.eof()){
    double kk_, Pk_;
    inf >> kk_ >> Pk_;
    kk.push_back(kk_);
    Pk.push_back(Pk_);
  }
  inf.close();
  generate_data(kk,Pk);
};



PowerSpecSource::~PowerSpecSource() {
  fftwf_free(buffer);
}


void PowerSpecSource::generate_data(const std::vector<double> &kk, const std::vector<double> &Pk) {
  size_t N = Nblocks*block_size;
  size_t Nfft = N/2+1;
  fftwf_complex* fourier = fftwf_alloc_complex(Nfft);
  buffer = fftwf_alloc_real(N);
  bool align_out = fftwf_alignment_of((float*)buffer);
  std::cout << "Planning " <<Nfft <<std::endl;
  fftwf_set_timelimit(1.0);
  fftwf_plan plan = fftwf_plan_dft_c2r_1d(N, fourier, buffer, FFTW_DESTROY_INPUT || FFTW_ESTIMATE);
  std::cout << "Generating " <<std::endl;
  // now generate fourier variates
  size_t ki=0;
  size_t kj=1;
  float domega = sampling_rate/N/1e6; // we work in MHz
  
  fourier[0][0] = fourier[0][1]=0; // DC component
  std::default_random_engine generator;
  std::normal_distribution<double> gauss(0.0,1.0);

  size_t NUp = N/2 + (second_fourier*N/2);

  double Pnorm = sqrt(sampling_rate/N/1e2);
  for (size_t i=1;i<=NUp;i++) {
    float omega = domega*i;
    size_t j;
    while (omega>kk[kj]) {ki++; kj++;}
    // now interpolate / extrapolate
    double Phere = Pk[ki]+(Pk[kj]-Pk[ki])/(kk[kj]-kk[ki])*(omega-kk[ki]); // in dvar/Hz
    double A = sqrt(Phere/2)*Pnorm; // fix norm factors
    if (i<=Nfft) j=i; else j = N-i;
    fourier[j][0]=A*gauss(generator);
    fourier[j][1]=A*gauss(generator);
    //    if (A>0.5)
    //  std::cout<<omega<<" " << A <<" " <<kk[ki] <<" " <<fourier[j][0]<<std::endl;
  }
  
  std::cout << "Transforming " <<std::endl;

  std::cout <<buffer[0]<<" " <<buffer[1]<<std::endl;
  fftwf_execute(plan);
  std::cout <<buffer[0]<<" " <<buffer[1]<<std::endl;
  fftwf_free(fourier);
  fftwf_destroy_plan(plan);
  std::cout << "Done " <<std::endl;

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

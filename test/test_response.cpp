
#include "RefSpectrometer.h"
#include "SignalGenerator.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

// CLI aruments handling:
#include "lyra.hpp"


// CLI aruments handling:
#include "lyra.hpp"

int main(int argc, char *argv[]) {

  bool verbose        =   false;
  bool cimode         =   false;

  size_t taps_start   = 4;
  size_t taps_end     = 4;

  size_t notch_start  = 0;
  size_t notch_end    = 1;

  size_t win_start    = 0;
  size_t win_end      = 3;

  auto cli = lyra::cli()
      | lyra::opt(verbose)
        ["-v"]["--verbose"]("verbose" )
      | lyra::opt(cimode)
        ["-c"]["--cimode"]("cimode" )        
      | lyra::opt(taps_start, "taps_start" )
        ["-s"]["--taps_start"]("taps_start")
      | lyra::opt(taps_end,   "taps_end" )
        ["-e"]["--taps_end"]("taps_end")
      | lyra::opt(notch_start, "notch_start" )
        ["-d"]["--notch_start"]("notch_start")
      | lyra::opt(notch_end,   "notch_end" )
        ["-r"]["--notch_end"]("notch_end")
      | lyra::opt(win_start, "win_start" )
        ["-f"]["--win_start"]("win_start")
      | lyra::opt(win_end,   "win_end" )
        ["-t"]["--win_end"]("win_end")
     ;

    auto result = cli.parse( { argc, argv } );
    if ( !result ) {
            std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
            exit(1);
    }

  if(verbose) {std::cout << "*** Verbose mode activated ***" << std::endl;}

  if(cimode) {
    taps_start  = taps_end  = 4;
    notch_start = notch_end = 0;
    win_start   = win_end   = 3;
  }

  if(verbose) {
    if(cimode) {std::cout << "*** Switching to CI mode ***" << std::endl;}
    std::cout << "Taps start:  " << taps_start  <<"       Taps end:"  << taps_end   << std::endl;
    std::cout << "Notch start: " << notch_start <<"       Notch end:" << notch_end  << std::endl;
    std::cout << "Win start:   " << win_start   <<"       Win end:"   << win_end    << std::endl;    
  }

  if(taps_start==0 || taps_end==0) {
    if(verbose) {std::cout << "*** Error: Specify taps_start and taps_end at the command line. ***" << std::endl;}
  }


  // Finished parsing, starting setup:
  SpecConfig cfg;

  cfg.Ntaps       = 7;
  cfg.Nchannels   = 1;
  cfg.AverageSize = 64;

  double fundamental = cfg.fundamental_frequency();

  // Let's do 1 second worth of data
  size_t blocks   = cfg.AverageSize+2*cfg.Ntaps;
  float Ampl      = 10;
  float noiseA    = 0.0;
  double central  = 10; // doesn't really matter

  for (size_t Ntaps=taps_start; Ntaps<=taps_end; Ntaps++) {

    if(verbose) {std::cout << "Doing taps: " << Ntaps << std::endl;}
    
    for (int notch = notch_start; notch <= notch_end; notch++) {
      for (int win = win_start; win <= win_end; win++) {
	std::stringstream fname;
	
	if(cimode) {
	  fname << "response_test.dat";
	}
	else {
	  fname << "response_" << Ntaps <<"_"<<notch<<"_"<<win<<".dat";
	}
      
      std::ofstream outfile;
      outfile.open(fname.str());

      cfg.Ntaps = Ntaps;
      cfg.notch = notch;
      cfg.window = (window_t)(win);

      for (double freq = central -3 ; freq< central + 3; freq+=0.01) {
  	    SignalGenerator signal(cfg.Nfft, cfg.Nchannels, blocks, freq*fundamental, cfg.sampling_rate, Ampl, noiseA);
	      RefSpectrometer S(&signal,&cfg);
	      
        SpecOutput O(&cfg);
	      S.run(&O);
	      outfile <<freq-central << " " << O.avg_pspec[0][10] << std::endl;
      }
      
      outfile.close();
      }
    }
  }

return 0;
}


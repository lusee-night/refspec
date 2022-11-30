
#include "RefSpectrometer.h"
#include "SignalGenerator.h"
#include "SpecConfig.h"
#include "SpecOutput.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

// -----------------------------------------------------
int main(int argc, char *argv[]) {

    size_t taps_start   = 4;
    size_t taps_end     = 4;

    size_t notch_start  = 0;
    size_t notch_end    = 0;

    size_t win_start    = 3;
    size_t win_end      = 3;

    std::cout << "Taps start:  " << taps_start  <<"       Taps end:"  << taps_end   << std::endl;
    std::cout << "Notch start: " << notch_start <<"       Notch end:" << notch_end  << std::endl;
    std::cout << "Win start:   " << win_start   <<"       Win end:"   << win_end    << std::endl;    

    // Finished parsing, starting setup:
    SpecConfig cfg;

    cfg.Ntaps       = 7;
    cfg.Nchannels   = 1;
    cfg.Average1Size = 64;
    cfg.Average2Size = 1;


    double fundamental = cfg.fundamental_frequency();


    // Let's do 1 second worth of data
    size_t blocks   = cfg.AverageSize()+2*cfg.Ntaps;
    float Ampl      = 10;
    float noiseA    = 0.0;
    double central  = 10; // doesn't really matter


    // std::cout << "*** Finishing ***" << std::endl;
    // exit(0);

    for (size_t Ntaps=taps_start; Ntaps<=taps_end; Ntaps++) {
        for (int notch = notch_start; notch <= notch_end; notch++) {
            for (int win = win_start; win <= win_end; win++) {
	            std::stringstream fname;
		        fname << "leak_test.dat";

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


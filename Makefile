CXX = g++
CXXFLAGS = -Ofast -g -std=c++17 -fopenmp -static
#CXXFLAGS = -D_GLIBCXX_DEBUG -g -Wall -Wno-sign-compare -Wno-reorder -std=c++17


lib_ext := $(shell python3.10-config --extension-suffix)

# -mxp-
PB11_CXXFLAGS = -fPIC -g

FFTW_LINK = -lfftw3 -lfftw3f
#FFTW_LINK = -lfftw3f_omp  -lfftw3f -lm
LINKFLAGS = 

SOURCES = src/pfb.cpp src/SpecConfig.cpp src/SpecOutput.cpp src/SignalGenerator.cpp \
          src/RefSpectrometer.cpp src/FileStreamSource.cpp src/PowerSpecSource.cpp \
	  src/SignalCombiner.cpp src/CombSource.cpp src/WhiteNoise.cpp

# -mxp-
PB11_SOURCES = src/pfb.cpp src/SpecConfig.cpp src/SpecOutput.cpp src/SignalGenerator.cpp \
          src/RefSpectrometer.cpp

OBJS = $(SOURCES:.cpp=.o)

# -mxp-
PB11_OBJS = $(PB11_SOURCES:.cpp=.o)

TEST_SOURCES = test/test_pfb.cpp test/test_timing.cpp test/test_response.cpp \
	test/simple_demo.cpp test/response_leak_detector.cpp test/psp_run.cpp \
	test/test_powspec_src.cpp calibrator_review22/calib.cpp calibrator_review22/drift_pred.cpp

TEST_EXECS = $(TEST_SOURCES:.cpp=.exe)

LIBRARY = refspec.a

# -mxp-

PB11_LIBRARY = refspec$(lib_ext).so

all: $(LIBRARY) $(TEST_EXECS)

pb11: $(PB11_LIBRARY)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -Iinclude  $< -o $@

# -mxp-
$(PB11_OBJS): %.o: %.cpp
	$(CXX) -c $(PB11_CXXFLAGS) -Iinclude -Iextern -I/usr/local/include/python3.10 $< -o $@

#
$(TEST_EXECS): %.exe: %.cpp $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -Iinclude   $< $(LIBRARY) $(FFTW_LINK) -o $@ 


$(LIBRARY): $(OBJS) Makefile 
	rm -f $(LIBRARY)
	ar rcs $(LIBRARY) $(OBJS)


# -mxp-
$(PB11_LIBRARY): $(PB11_OBJS) Makefile
	rm -f $(PB11_LIBRARY)
	ar rcs $(PB11_LIBRARY) $(PB11_OBJS)


pb11_clean:
	rm -f $(PB11_LIBRARY) $(PB11_OBJS)

clean:
	rm -f $(LIBRARY) $(OBJS)


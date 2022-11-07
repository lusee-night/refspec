CXX = g++
CXXFLAGS = -Ofast -g -std=c++17 -fopenmp -static
#CXXFLAGS = -D_GLIBCXX_DEBUG -g -Wall -Wno-sign-compare -Wno-reorder -std=c++17

FFTW_LINK = -lfftw3 -lfftw3f
#FFTW_LINK = -lfftw3f_omp  -lfftw3f -lm
LINKFLAGS = 

SOURCES = src/pfb.cpp src/SpecConfig.cpp src/SpecOutput.cpp src/SignalGenerator.cpp \
          src/RefSpectrometer.cpp src/FileStreamSource.cpp src/PowerSpecSource.cpp \
	  src/SignalCombiner.cpp src/CombSource.cpp src/WhiteNoise.cpp

OBJS = $(SOURCES:.cpp=.o)

TEST_SOURCES = test/test_pfb.cpp test/test_timing.cpp test/test_response.cpp \
	test/simple_demo.cpp test/response_leak_detector.cpp test/psp_run.cpp \
	test/test_powspec_src.cpp review/calib.cpp

TEST_EXECS = $(TEST_SOURCES:.cpp=.exe)

LIBRARY = refspec.a

all: $(LIBRARY) $(TEST_EXECS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -Iinclude  $< -o $@

$(TEST_EXECS): %.exe: %.cpp $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -Iinclude   $< $(LIBRARY) $(FFTW_LINK) -o $@ 


$(LIBRARY): $(OBJS) Makefile 
	rm -f $(LIBRARY)
	ar rcs $(LIBRARY) $(OBJS)

clean:
	rm $(LIBRARY) $(OBJS)


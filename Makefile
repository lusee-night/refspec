CXX = g++
CXXFLAGS_STATIC = -Ofast -g -std=c++17 -fopenmp -static
#CXXFLAGS_STATIC= -D_GLIBCXX_DEBUG -g -Wall -Wno-sign-compare -Wno-reorder -std=c++17

FFTW_LINK = -lfftw3 -lfftw3f  # older version: FFTW_LINK = -lfftw3f_omp  -lfftw3f -lm

LINKFLAGS = 

SOURCES = src/pfb.cpp src/SpecConfig.cpp src/SpecOutput.cpp src/SignalGenerator.cpp \
          src/RefSpectrometer.cpp src/FileStreamSource.cpp src/PowerSpecSource.cpp \
	  	  src/SignalCombiner.cpp src/CombSource.cpp src/WhiteNoise.cpp 

OBJS = $(SOURCES:.cpp=.o)

# -mxp- keep as reference, use deprecated - PB11_OBJS = $(PB11_SOURCES:.cpp=.o)

TEST_SOURCES = test/test_pfb.cpp test/test_timing.cpp test/test_response.cpp \
	test/simple_demo.cpp test/response_leak_detector.cpp test/psp_run.cpp \
	test/test_powspec_src.cpp calibrator_review22/calib.cpp calibrator_review22/drift_pred.cpp \
	test/test_bin_output.cpp

TEST_EXECS = $(TEST_SOURCES:.cpp=.exe)

# Main targets
LIBRARY = refspec.a

all: $(LIBRARY) $(TEST_EXECS)
$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS_STATIC) -Iinclude  $< -o $@


$(TEST_EXECS): %.exe: %.cpp $(LIBRARY)
	$(CXX) $(CXXFLAGS_STATIC) $(LINKFLAGS) -Iinclude   $< $(LIBRARY) $(FFTW_LINK) -o $@ 

$(LIBRARY): $(OBJS) Makefile 
	rm -f $(LIBRARY)
	ar rcs $(LIBRARY) $(OBJS)

clean:
	rm -f $(LIBRARY) $(OBJS)



########################################################################
# -mxp- Original code, in a slight conflict with previous rule, so comment out for now
#$(PB11_LIBRARY):  $(PB11_OBJS) Makefile
#	rm -f $(PB11_LIBRARY)
#	ar rcs $(PB11_LIBRARY) $(PB11_OBJS)
#pb11_clean:
#	rm -f $(PB11_LIBRARY) $(PB11_OBJS)
#$(PB11_OBJS):  %.o: %.cpp
#	@[ -d "extern/pybind11/pybind11" ] || (echo "---\nInstallation of pybind11 appears to be missing\nPlease consult README.md for instructions\nExiting...\n---"; exit 1;)
#	$(CXX) -c $(CXXFLAGS_DYNAMIC) -Iinclude -Iextern ${python_includes} $< -o $@

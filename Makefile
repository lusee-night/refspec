CXX = g++
CXXFLAGS = -Ofast -g -std=c++17 -fopenmp -static
#CXXFLAGS = -D_GLIBCXX_DEBUG -g -Wall -Wno-sign-compare -Wno-reorder -std=c++17

# Define convenience variables for use in the pb11 build
lib_ext := $(shell python3.10-config --extension-suffix)
python_includes := $(shell python3.10 -m pybind11 --includes)

# -mxp-
PB11_CXXFLAGS = -O3 -fPIC -g -std=c++17
# -lfftw3 -lfftw3f

PB11_BUILD = ./pb11_build

FFTW_LINK = -lfftw3 -lfftw3f  # older version: FFTW_LINK = -lfftw3f_omp  -lfftw3f -lm

LINKFLAGS = 

SOURCES = src/pfb.cpp src/SpecConfig.cpp src/SpecOutput.cpp src/SignalGenerator.cpp \
          src/RefSpectrometer.cpp src/FileStreamSource.cpp src/PowerSpecSource.cpp \
	  	  src/SignalCombiner.cpp src/CombSource.cpp src/WhiteNoise.cpp

# Deprecated: PB11_SOURCES = src/refspec.cpp src/pfb.cpp src/SpecConfig.cpp src/SpecOutput.cpp src/SignalGenerator.cpp src/RefSpectrometer.cpp

# -mxp- some definitions for pb11
PB11_SRCS = refspec.cpp pfb.cpp SpecConfig.cpp SpecOutput.cpp SignalGenerator.cpp RefSpectrometer.cpp PowerSpecSource.cpp
OBJ_FILES := $(patsubst %.cpp, $(PB11_BUILD)/%.o, $(PB11_SRCS) )

OBJS = $(SOURCES:.cpp=.o)

# -mxp- keep as reference, use deprecated - PB11_OBJS = $(PB11_SOURCES:.cpp=.o)

TEST_SOURCES = test/test_pfb.cpp test/test_timing.cpp test/test_response.cpp \
	test/simple_demo.cpp test/response_leak_detector.cpp test/psp_run.cpp \
	test/test_powspec_src.cpp calibrator_review22/calib.cpp calibrator_review22/drift_pred.cpp

TEST_EXECS = $(TEST_SOURCES:.cpp=.exe)

# Main targets
LIBRARY = refspec.a
PB11_LIBRARY = refspec$(lib_ext)

all: $(LIBRARY) $(TEST_EXECS) pb11

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -Iinclude  $< -o $@

$(PB11_BUILD):
	$(shell mkdir -p $(PB11_BUILD))

$(TEST_EXECS): %.exe: %.cpp $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -Iinclude   $< $(LIBRARY) $(FFTW_LINK) -o $@ 

$(LIBRARY): $(OBJS) Makefile 
	rm -f $(LIBRARY)
	ar rcs $(LIBRARY) $(OBJS)

clean:
	rm -f $(LIBRARY) $(OBJS) $(PB11_LIBRARY)
	rm -fr $(PB11_BUILD)

pb11: $(PB11_BUILD) $(OBJ_FILES) Makefile
	@echo Building the shared library for PYBIND11
	g++ $(OBJ_FILES) -lfftw3 -lfftw3f -shared -o $(PB11_LIBRARY) 
	rm -fr $(PB11_BUILD)

$(OBJ_FILES): $(PB11_BUILD)/%.o: src/%.cpp Makefile
	@echo Building object files for PYBIND11
	@[ -d "extern/pybind11/pybind11" ] || (echo "---\nInstallation of pybind11 appears to be missing\nPlease consult README.md for instructions\nExiting...\n---"; exit 1;)	
	$(CXX) -c $(PB11_CXXFLAGS) -Iinclude -Iextern ${python_includes} $< -o $@


########################################################################
# -mxp- Original code, in a slight conflict with previous rule, so comment out for now
#$(PB11_LIBRARY):  $(PB11_OBJS) Makefile
#	rm -f $(PB11_LIBRARY)
#	ar rcs $(PB11_LIBRARY) $(PB11_OBJS)
#pb11_clean:
#	rm -f $(PB11_LIBRARY) $(PB11_OBJS)
#$(PB11_OBJS):  %.o: %.cpp
#	@[ -d "extern/pybind11/pybind11" ] || (echo "---\nInstallation of pybind11 appears to be missing\nPlease consult README.md for instructions\nExiting...\n---"; exit 1;)
#	$(CXX) -c $(PB11_CXXFLAGS) -Iinclude -Iextern ${python_includes} $< -o $@

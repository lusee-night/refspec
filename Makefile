CXX = g++

CXXFLAGS_DYNAMIC = -O3 -fPIC -g -std=c++17 
#-D_GLIBCXX_DEBUG

BUILD = ./build

FFTW_LINK = -lfftw3 -lfftw3f

SRCS = pfb.cpp SpecConfig.cpp SpecOutput.cpp SignalGenerator.cpp SignalCombiner.cpp RefSpectrometer.cpp PowerSpecSource.cpp CombSource.cpp FileStreamSink.cpp WhiteNoise.cpp FileStreamSource.cpp CorrelatedSpecSource.cpp 

HDRS = SpecConfig.h SpecOutput.h SignalGenerator.h SignalCombiner.h RefSpectrometer.h PowerSpecSource.h CombSource.h CorrelatedSpecSource.h 

OBJS := $(patsubst %.cpp, $(BUILD)/%.o, $(SRCS) )

LIBRARY = refspec.so

all: LIBRARY

$(BUILD):
	$(shell mkdir -p $(BUILD))

clean:
	rm -f $(LIBRARY)
	rm -fr $(BUILD)

LIBRARY: $(BUILD) $(OBJS) Makefile
	@echo Building the refspec shared library
	g++ $(OBJS) -shared -lfftw3 -lfftw3f -o $(LIBRARY) 


$(OBJS): $(BUILD)/%.o: src/%.cpp include/%.h Makefile
	$(CXX) -c $(CXXFLAGS_DYNAMIC) -Iinclude -Iextern ${python_includes} $< -o $@


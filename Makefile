CXX = g++
CXXFLAGS = -Ofast -g

FFTW_LINK = -lfftw3 -lfftw3f 
LINKFLAGS = -static

SOURCES = src/pfb.cpp src/SpecConfig.cpp src/SpecOutput.cpp src/SignalGenerator.cpp \
          src/RefSpectrometer.cpp

OBJS = $(SOURCES:.cpp=.o)

<<<<<<< HEAD
TEST_SOURCES = test/test_pfb.cpp test/test_timing.cpp test/test_response.cpp test/test_demo.cpp
TEST_EXECS = $(TEST_SOURCES:.cpp=.out)
=======
TEST_SOURCES = test/test_pfb.cpp test/test_timing.cpp test/test_response.cpp
TEST_EXECS = $(TEST_SOURCES:.cpp=.exe)
>>>>>>> 53dbf01213285f718751d8dff40de760a422674a

LIBRARY = refspec.a

all: $(LIBRARY) $(TEST_EXECS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -Iinclude  $< -o $@

$(TEST_EXECS): %.exe: %.cpp $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -Iinclude   $< $(LIBRARY) $(FFTW_LINK)-o $@ 


$(LIBRARY): $(OBJS) Makefile 
	rm -f $(LIBRARY)
	ar rcs $(LIBRARY) $(OBJS)

clean:
	rm $(LIBRARY) $(OBJS)


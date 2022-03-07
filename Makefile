CXX = g++
CXXFLAGS = -Ofast

# -L/usr/local/lib/
FFTW_LINK = -lfftw3 -lfftw3f 
LINKFLAGS = -static

SOURCES = src/pfb.cpp
OBJS = $(SOURCES:.cpp=.o)

TEST_SOURCES = test/test_pfb.cpp
TEST_EXECS = $(TEST_SOURCES:.cpp=.out)

LIBRARY = refspec.a

all: $(LIBRARY) $(TEST_EXECS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -Iinclude  $< -o $@

$(TEST_EXECS): %.out: %.cpp $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) -Iinclude   $< $(LIBRARY) $(FFTW_LINK)-o $@ 


$(LIBRARY): $(OBJS) Makefile 
	rm -f $(LIBRARY)
	ar rcs $(LIBRARY) $(OBJS)



clean:
	rm $(LIBRARY) $(OBJS)


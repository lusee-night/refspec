CXX = g++
CXXFLAGS_DYNAMIC = -O3 -fPIC -g -std=c++17

calib.exe: calib.cpp
	$(CXX) $(CXXFLAGS_DYNAMIC) -Iinclude -Iextern -L.. -I../include  $< -o $@ -lrefspec 

#Declaracion de Variables
CXX=g++
CXXFLAGS=-std=c++11 -O2 -o executable

all: compile


compile: elDE.h elDE.cc elDE_SHADE.h elDE_SHADE.cc elDE_EDM.h elDE_EDM.cc elDE_EDM_v2.h elDE_EDM_v2.cc elDE_EDM_C.h  elDE_EDM_C.cc cec17_test_COP.cpp  extern.cc main.cc
	$(CXX) $(CXXFLAGS)  elDE.cc elDE_SHADE.cc elDE_EDM.cc elDE_EDM_v2.cc elDE_EDM_C.cc cec17_test_COP.cpp extern.cc main.cc

clean:
	rm -f executable

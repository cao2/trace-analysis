CXX=g++
CXXFLAGS=-I__SYSCPATH__/include #-O2
LDFLAGS=-L__SYSCPATH__/lib-__TARGETHOST__ -lsystemc
#LDFLAGS=-lsystemc


.PHONY: clean all

all: instlink

clean:
	$(RM) instlink

instlink:  dumbfooexploiter.cpp dumbfooimplementor.cpp fooif.hpp main.cpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(LDFLAGS)


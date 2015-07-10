CXX=g++
CXXFLAGS=-I__SYSCPATH__/include #-O2
LDFLAGS=-L__SYSCPATH__/lib-__TARGETHOST__ -lsystemc
#LDFLAGS=-lsystemc


.PHONY: clean all

all: refinedmemcpu

clean:
	$(RM) refinedmemcpu

refinedmemcpu: mem_if.hpp memory.cpp cpu.cpp main.cpp cpuadaptor_signals.cpp memadaptor_signals.cpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(LDFLAGS)

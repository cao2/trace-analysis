CXX=g++
CXXFLAGS=-I__SYSCPATH__/include #-O2
LDFLAGS=-L__SYSCPATH__/lib-__TARGETHOST__ -lsystemc


.PHONY: clean all

all: buffer

clean:
	$(RM) buffer

buffer: buffer_if.hpp flatbuffer.cpp complexbuffer.cpp bufwriter.cpp main.cpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(LDFLAGS)


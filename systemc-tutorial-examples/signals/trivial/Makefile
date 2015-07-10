CXXFLAGS=-I__SYSCPATH__/include #-O2
LDFLAGS=-L__SYSCPATH__/lib-__TARGETHOST__ -lsystemc

.PHONY: all clean

all: trivial

clean:
	$(RM) trivial

trivial: trivial.cpp
	$(CXX) $(CXXFLAGS) -o $@ trivial.cpp $(LDFLAGS)

CXXFLAGS=-I__SYSCPATH__/include #-O2
LDFLAGS=-L__SYSCPATH__/lib-__TARGETHOST__ -lsystemc

.PHONY: all clean

all: prodcons

clean:
	$(RM) prodcons

prodcons: main.cpp producer.cpp consumer.cpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(LDFLAGS)

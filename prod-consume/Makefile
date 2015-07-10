# Location of the SystemC library files
SYSTEMC_HOME    = $(HOME)/Dropbox/work/sw/systemc-2.3.1
SYSTEMC_LIB  = $(SYSTEMC_HOME)/lib-macosx64
SYSTEMC_INCLUDE = $(SYSTEMC_HOME)/include


# Compiler settings
CC              = g++
CFLAGS          = -Wall -O2
#INCLUDES        = -I$(SYSTEMC_INCLUDE) -I$(ACALIB_DIR)
#LIBS            = -lsystemc
#LIBDIR          = -L$(SYSTEMC_LIBDIR)


.PHONY: clean all

all: top

clean:
	rm top

top: write_if.cpp read_if.cpp fifo.cpp producer.cpp consumer.cpp top.cpp
	echo $@
	$(CC) $(CFLAGS) -I$(SYSTEMC_INCLUDE) -L$(SYSTEMC_LIB) -o $@ top.cpp -lsystemc


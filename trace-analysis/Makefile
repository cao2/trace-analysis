OBJS = analysis.o
SRC = $(OBJS:.o=.cc)


# Compiler settings
CC              = g++
CFLAGS          = -Wall -O2
#INCLUDES        = -I$(SYSTEMC_INCLUDE) -I$(ACALIB_DIR)
#LIBS            = -lsystemc
#LIBDIR          = -L$(SYSTEMC_LIBDIR)


.PHONY: clean all

all: $(OBJS)

analysis: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

analysis.o: msg_def.h lpn.h analysis.cc 
	$(CC) $(CFLAGS) -c  analysis.cc




clean:
	rm *.o
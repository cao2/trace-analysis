# This Makefile requires GNU Make 3.81 or newer

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

exec:
	$(CC) -I$(SYSTEMC_INCLUDE) -L$(SYSTEMC_LIB) -o sim hello.cpp -lsystemc -lm


.SECONDEXPANSION:
.PHONY: all targets clean $(TARGETS)

all: $(TARGETS)
	
$(TARGETS): $$@.bin

%.bin: $(D_CPP_FILES) $(D_H_FILES) $(SYSTEMC_LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(CPP_FILES) $(ACALIB) $(LIBDIR) $(LIBS)
	
targets:
	@echo List of found targets:
	@echo $(TARGETS)
	@echo
	@echo List of found cpp files:
	@echo $(foreach sourcedir,$(TARGETS),$(wildcard $(SOURCE_PATH)/$(sourcedir)/*.cpp))
	@echo
	@echo SystemC installation used in:
	@echo $(SYSTEMC_LIBDIR)        

clean:
	rm -f $(TARGETS:%=%.bin)


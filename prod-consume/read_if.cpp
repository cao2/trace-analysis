#ifndef _READ_IF_
#define _READ_IF_

#include "systemc.h"

class read_if : virtual public sc_interface {
public:
	virtual void read(char &) = 0; 
	virtual int num_available() = 0;
};

#endif

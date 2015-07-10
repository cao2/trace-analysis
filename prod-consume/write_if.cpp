#ifndef _WRITE_IF_
#define _WRITE_IF_

#include "systemc.h"

class write_if : virtual public sc_interface {
public:
	virtual void write(char) = 0; 
	virtual void reset() = 0;
};

#endif


#ifndef _COMM_IF_
#define _COMM_IF_

#include "systemc.h"
#include "msg_def.hpp"


/*
 * An interface for memory modules, which provide both unit and block
 * read/write operations.
 * The behaviour of complying modules when accessing addresses outside the
 * memory address space is not specified.
 * The behaviour of complying modules when reading from memory areas which
 * have not been previously written is not detailed either.
 */

class write_if : public sc_interface
{
public:
    virtual void write(message_t&) = 0;
    //virtual void reset() = 0;
};

class read_if : public sc_interface
{
public:
    virtual void read(message_t&) = 0;
    virtual void read_cond(message_t&, command_t) = 0;
    virtual void read_cond(message_t&, command_t, address_t addr) = 0;
    virtual void read_cond_select(message_t&, command_t, command_t) = 0;
    //virtual int num_available() = 0;
};

#endif


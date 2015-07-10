
#ifndef _MEM_IF_
#define _MEM_IF_

#include "systemc.h"

// define what type memory addresses resolve to
typedef unsigned long address_t;


/*
 * An interface for memory modules, which provide both unit and block
 * read/write operations.
 * The behaviour of complying modules when accessing addresses outside the
 * memory address space is not specified.
 * The behaviour of complying modules when reading from memory areas which
 * have not been previously written is not detailed either.
 */
class mem_if : public sc_interface {
    public:
        // return the size of the memory module
        virtual size_t size() = 0;

        // fetch one unit value at the given memory address into the given buffer
        virtual void read(char &data, address_t address) = 0;
        // store one given data unit at the given memory address
        virtual void write(const char data, address_t address) = 0;
        
        /*
         * read a block of data units from the memory, starting from address
         * "whence", for numbytes. Store such data into the buffer starting at
         * "data".
         */
        virtual void readblock(char data[], size_t numbytes, address_t whence) = 0;
        /*
         * store a block of data units into the memory, starting from memory
         * address "whence". Store numbytes units from the data given in
         * buffer "data".
         */
        virtual void writeblock(const char data[], size_t numbytes, address_t whence) = 0;
};

#endif


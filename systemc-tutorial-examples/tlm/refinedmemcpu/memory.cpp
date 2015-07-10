
#ifndef _MEMORY_
#define _MEMORY_

#include <stdexcept>
#include "systemc.h"
#include "mem_if.hpp"


// the number of atomic data units Memory can hold
#define MEMORYSIZE      100000

// you might want to try with:
// #define MEMORYSIZE      2


/*
 * Memory is an implementation of the mem_if interface.
 * Checks are done for read/write out of bounds. No checks
 * are done over the sanity of an area; that is, reads from
 * uninitialized areas are not rejected.
 */
class Memory : public sc_module, public mem_if {
    private:
        char *data;

        // checks whether address is a valid address for this memory module
        inline bool safeAddress(address_t address) const {
            return (address < MEMORYSIZE);
        }


    public:
        // module constructor
        SC_CTOR(Memory) {
            // allocate data for memory
            data = new char[MEMORYSIZE];
        }


        // module destructor
        ~Memory() {
            // free memory data
            delete[] data;
        }


        size_t size() {
            return MEMORYSIZE;
        }


        void read(char &data, address_t address) {
            // trying to read out of bounds?
            if (!safeAddress(address))
                throw std::runtime_error("Trying to read out of bounds.");

            // storing the value
            data = this->data[address];
        }


        void write(const char data, address_t address) {
            // trying to write out of bounds?
            if (!safeAddress(address))
                throw std::runtime_error("Trying to write out of bounds.");

            // storing the value
            this->data[address] = data;
        }


        void readblock(char data[], size_t numbytes, address_t whence) {
            // trying to read out of bounds?
            if (!safeAddress(whence + numbytes))
                throw std::runtime_error("Tried to read out of bounds.");

            // copy memory's data (from whence) into the *data buffer
            for (address_t count = 0; count < numbytes; count++) {
                data[count] = this->data[whence+count];
            }
        }


        void writeblock(const char data[], size_t numbytes, address_t whence) {
            // trying to write out of bounds?
            if (!safeAddress(whence + numbytes))
                throw std::runtime_error("Tried to write out of bounds.");

            // safe request. Store the content of *data into the memory
            for (address_t count = 0; count < numbytes; count++) {
                this->data[whence+count] = data[count];
            }
        }
};

#endif


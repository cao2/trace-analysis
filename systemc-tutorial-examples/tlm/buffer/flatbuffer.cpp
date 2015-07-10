
#ifndef _FLAT_BUFFER_
#define _FLAT_BUFFER_

#include <stdexcept>

#include "systemc.h"
#include "buffer_if.hpp"


/*
 * FlatBuffer is an actual (instantiable) implementation of the buffer_if
 * module interface.
 * The implementation provides a 1-slot buffer that fails when reading
 * from empty and when writing to full states.
 */
class FlatBuffer : public sc_module, public buffer_if {
    private:
        bufferdatatype data;
        bool busy;

    public:
        // FlatBuffer constructor
        SC_CTOR(FlatBuffer) {
            // the buffer is initially empty
            busy = false;
        }


        /*
         * store data in the buffer
         *
         * store data, if the buffer is free. Otherwise, raise
         * an exception.
         *
         * @param data      value to store
         */
        void write(bufferdatatype data) {
            if (busy)   // if something is held that hasn't been read yet
                throw std::runtime_error("FlatBuffer busy, read to free.");
            // otherwise: store the new data and mark the buffer busy
            this->data = data;
            busy = true;
        }

        
        /*
         * read data from the buffer
         *
         * read data from the buffer, is some is available. Otherwise, raise
         * an exception
         * 
         * @return data available
         */
        bufferdatatype read() {
            if (!busy)
                throw std::runtime_error("FlatBuffer empty.");
            busy = false;
            return data;
        }


        /*
         * clear the buffer
         *
         * clear the buffer from data stored, if any
         */
        void clear() {
            busy = false;
        }
};

#endif


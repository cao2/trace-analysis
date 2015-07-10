
#ifndef _COMPLEX_BUFFER_
#define _COMPLEX_BUFFER_

#include <stdexcept>

#include "systemc.h"
#include "buffer_if.hpp"

// how many data slots the ComplexBuffer must feature
#define CBUF_SLOTS       10


/*
 * ComplexBuffer is an actual (instantiable) implementation of the buffer_if
 * module interface.
 * The implementation provides a 1-slot buffer that fails when reading
 * from empty and when writing to full states.
 */
class ComplexBuffer : public sc_module, public buffer_if {
    private:
        bufferdatatype data[CBUF_SLOTS];
        unsigned int first, last;
        bool isEmpty() { return (first == last); }
        bool isFull() { return ((last + 1) % CBUF_SLOTS ==  first); }
        void reset() { first = last = 0; }

    public:
        // ComplexBuffer constructor
        SC_CTOR(ComplexBuffer) {
            // the buffer is initially empty
            reset();
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
            if (isFull()) {
                throw std::runtime_error("ComplexBuffer full, read to free.");
            }
            // otherwise: store the new data and mark the buffer busy
            this->data[last] = data;
            last = (last + 1) % CBUF_SLOTS;
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
            if (isEmpty())
                throw std::runtime_error("ComplexBuffer empty, write to fill.");
            unsigned int tmpfirst = first;
            first = (first + 1) % CBUF_SLOTS;
            return data[tmpfirst];
        }


        /*
         * clear the buffer
         *
         * clear the buffer from data stored, if any
         */
        void clear() {
            reset();
        }
};

#endif


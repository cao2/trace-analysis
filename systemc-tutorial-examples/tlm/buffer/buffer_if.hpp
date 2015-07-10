
#ifndef _BUS_HPP_
#define _BUS_HPP_

/* what data is exchanged through the buffer */
typedef int  bufferdatatype;

/* 
 * The interface for buffer modules.
 */
class buffer_if : public sc_interface {
    public:
        // write data onto the buffer
        virtual void write(bufferdatatype data) = 0;
        // read data from the buffer
        virtual bufferdatatype read() = 0;
        // clear the buffer
        virtual void clear() = 0;
};

#endif



#ifndef _CPUADAPTOR_SIGNALS_
#define _CPUADAPTOR_SIGNALS_

#include <stdexcept>

#include "systemc.h"

#include "mem_if.hpp"

/*
 * A CPUAdaptor_Signals is provided to a CPU memory port to let such CPU
 * communicate with a memory module.
 * The underlying mechanism for communication is hidden to the CPU; this
 * module appears like a memory itself, as it implements the mem_if interface.
 */
class CPUAdaptor_Signals : public sc_channel, public mem_if {
    private:
        void waitendptfree() {
            while (ack.read() != false) wait(ack.value_changed_event());
        }
        bool waitresultready() {
            bool status;
            while ((status = ack.read()) != true && err.read() != true)
                wait(ack.value_changed_event() | err.value_changed_event());
            return status; // ack true => no errors
        }

    public:
        // bus wires
        sc_inout<address_t> addr;
        sc_inout<char> data;
        sc_out<bool> memsize;    // get size
        sc_out<bool> r_w;  // T: read / F: write
        sc_out<bool> req;
        sc_in<bool> ack;
        sc_out<bool> err;


        SC_CTOR(CPUAdaptor_Signals) {
            sensitive << ack;
        }


        // asking memory for what's its capacity
        size_t size() {
            // memory size is not necessarily sizeof(address_t),
            // so this value must be provided directly to the memory
            waitendptfree();
            size_t sz;
            // raise request for size
            memsize.write(true);
            req.write(true);
            // wait for receiver to acknowledge
            waitresultready();
            sz = addr.read();
            // bring back bus to ready state
            memsize.write(false);
            req.write(false);

            return sz;
        }

        
        void read(char &data, address_t address) {
            // wait for the receiver to become available
            waitendptfree();
            addr.write(address);
            r_w.write(true);
            req.write(true);
            // wait for the receiver to acknowledge the request
            if (waitresultready() == false) {
                // an error occurred, propagate to CPU
                throw std::runtime_error("Error while reading.");
            }
            // service accomplished, data available on bus
            data = this->data.read();
            // recognizing the service
            req.write(false);
        }


        void write(const char data, address_t address) {
            // wait for the receiver to become available
            waitendptfree();
            // prepare info on bus to issue a write request
            addr.write(address);
            this->data.write(data);
            r_w.write(false);
            req.write(true);
            // wait for receiver to acknowledge de request
            if (waitresultready() == false) {
                // an error occurred, propagate to CPU
                throw std::runtime_error("Error while writing");
            }
            req.write(false);
        }

        void readblock(char data[], size_t numbytes, address_t whence) {
            for (address_t i = 0; i < numbytes; i++) read(data[i], whence+i);
        }

        void writeblock(const char data[], size_t numbytes, address_t whence) {
            for (address_t i = 0; i < numbytes; i++) write(data[i], whence+i);
        }
};

#endif



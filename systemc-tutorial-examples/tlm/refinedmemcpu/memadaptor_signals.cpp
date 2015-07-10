
#ifndef _MEMADAPTOR_SIGNALS_
#define _MEMADAPTOR_SIGNALS_

#include <iostream>
#include <stdexcept>

#include "systemc.h"
#include "mem_if.hpp"

/*
 * A MemAdaptor_Signals masquerades the bus-like communication to a Memory
 * module. It translates the signals communication style back to the
 * procedure-call style.
 */
class MemAdaptor_Signals : public sc_module {
    public:
        sc_inout<address_t> addr;
        sc_inout<char> data;
        sc_in<bool> memsize;        // get size
        sc_in<bool> r_w;            // T: read / F: write
        sc_in<bool> req;
        sc_out<bool> ack;
        sc_out<bool> err;

        
        sc_port<mem_if> memory;     // port for memory (TLM)
        
        SC_CTOR(MemAdaptor_Signals) {
            SC_THREAD(act);
            // make the thread sensible to changes on the req port
            sensitive << req;
        }

        void act() {
            while (true) {
                cout << "MemAdaptor: waiting for requests..." << endl;
                // wait for a request to come
                while (req.read() != true) wait();
                // request now accepted
                address_t memaddr = addr.read(); // read recipient address
                // SIZE request?
                if (memsize.read() == true) {
                    // SIZE request!
                    cout << "MemAdaptor: SIZE request income, processing...";
                    addr.write(memory->size());
                    cout << " done." << endl;
                    goto act_oper_ok;
                }
                // READ request?
                if (r_w.read() == true) {
                    // READ request!
                    cout << "MemAdaptor: READ request income, processing...";
                    char val;
                    try {
                        memory->read(val, memaddr);
                    } catch (std::runtime_error &e) {
                        // an error occurred, reporting on the bus
                        goto act_oper_err;
                    }
                    data.write(val);
                    cout << " done." << endl;
                    goto act_oper_ok;
                }
                // in any other case..
                // WRITE request!
                cout << "MemAdaptor: WRITE request income, processing...";
                try { memory->write(data.read(), memaddr); } catch (std::runtime_error &e) {
                    // an error occurred, reporting on the bus
                    goto act_oper_err;
                }
                cout << " done." << endl;
                goto act_oper_ok;
act_oper_err:       // an error happened operating the service
                err.write(true);
                while (req.read() != false) wait();
                err.write(false);
                continue;

act_oper_ok:        // service went ok
                ack.write(true);    // request served
                // wait for peer assertion
                while (req.read() != false) wait();
                ack.write(false);   // terminate the operation
            }
        }
};

#endif


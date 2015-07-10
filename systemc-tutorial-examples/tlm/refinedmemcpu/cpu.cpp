
#ifndef _CPU_
#define _CPU_

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

#include "systemc.h"

#include "mem_if.hpp"

/*
 * A CPU generates some service requests to a memory.
 */
class CPU : public sc_module {
    private:
        unsigned int memsize;

    public:
        sc_port<mem_if> memory;

        SC_CTOR(CPU) {
            srandom(time(NULL));
            SC_THREAD(act);
        }

        void act() {
            // get aware of what capacity the memory features
            memsize = memory->size();
            cout << "CPU: size request served, capacity is " << memsize << " data units." << endl;

            // make room for local computations
            const unsigned char opbufsize = 10;  // set this > 2!
            char *operate_buf = new char[opbufsize];
            // initialize operate_buf with some readable (ascii) data
            for (char *x = operate_buf; x < operate_buf + opbufsize; x++) {
                *x = 48 + (random() % 43); // some in the ascii alphanum domain (circa)
            }

            // A unit operation -- store operate_buf[0] into memory at some random address
            // choose a random address (within memory space) to operate, warn, then write
            address_t operaddr = random() % memsize;
            cout << "CPU: will write " << operate_buf[0] << " into memory's " << operaddr << " cell." << endl;
            // write and read ops might cause errors...
            try {
                memory->write(operate_buf[0], operaddr);
            } catch (std::runtime_error &e) {
                cout << "CPU: an error occurred: " << e.what() << endl;
                return;
            }
            char tmpbuf;
            try {
                memory->read(tmpbuf, operaddr);
            } catch (std::runtime_error &e) {
                cout << "CPU: an error occurred: " << e.what() << endl;
                return;
            }
            cout << "CPU: memory location " << operaddr << " now holding '" << tmpbuf << "'." << endl;

            // A block operation -- store a bunch of data units into memory at some
            // random address, then, fetch some of them back
            operaddr = random() % (memsize - opbufsize);
            // advicing data, and writing to memory
            cout << "CPU: will write: "; for (unsigned char i = 0; i < opbufsize; i++) cout << operate_buf[i]; cout << endl;
            try {
                memory->writeblock(operate_buf, opbufsize, operaddr);
            } catch (std::runtime_error &e) {
                cout << "CPU: an error occurred: " << e.what() << endl;
                return;
            }
            // reading back some piece of it
            cout << "CPU: getting back " << opbufsize/2 << " units of data...";
            char *newbuf = new char[opbufsize/2];
            try {
                memory->readblock(newbuf, opbufsize/2, operaddr);
            } catch (std::runtime_error &e) {
                cout << "CPU: an error occurred: " << e.what() << endl;
                return;
            }

            cout << "CPU: got back ";
            for (unsigned char i = 0; i < opbufsize/2; i++) cout << newbuf[i];
            cout << endl;

            delete[] operate_buf;   // free the temporary buffer
        }
};

#endif


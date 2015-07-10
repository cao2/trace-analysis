
#ifndef _BUFWRITER_
#define _BUFWRITER_

#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include "systemc.h"

#include "buffer_if.hpp"

/*
 * Bufwriter implements a module that exploits a buffer's service.
 * In its activity, it randomly issues store or fetch operations
 * to the buffer. In case of failure of an operation, a message is
 * displayed.
 */
class Bufwriter : public sc_module {
    public:
        sc_port<buffer_if> buf;     // port for the buffer to exploit


        // module constructor
        SC_CTOR(Bufwriter) {
            // seed the random number generator
            srandom(time(NULL));
            SC_THREAD(act);
        }
        

        /*
         * activity of this module:
         * repeatedly issue write or read commands to the
         * buffer, randomly
         */
        void act() {
            unsigned int somenum;
            
            while (1) {
                somenum = random() % 100000;
                usleep(somenum);
                if (somenum % 2)    // try writing
                    try {
                        buf->write(0);
                        cout << "Write OK." << endl;
                    } catch (std::exception &e) { cout << "Buffer was busy." << endl; }
                else                // try reading
                    try {
                        buf->read();
                        cout << "Read OK." << endl;
                    } catch (std::exception &e) { cout << "Buffer was empty." << endl; }
            }
        }
};

#endif


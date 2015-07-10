
#ifndef _MEMORY_
#define _MEMORY_

#include <stdexcept>
#include "comm_if.hpp"
#include "msg_def.hpp"


// the number of atomic data units Memory can hold
#define MEMORYSIZE      0x0000FFFF


/*
 * Memory is an implementation of the mem_if interface.
 * Checks are done for read/write out of bounds. No checks
 * are done over the sanity of an area; that is, reads from
 * uninitialized areas are not rejected.
 */
class Memory : public sc_module {
private:
    sc_event msgin_ready;
    
public:
    sc_port<read_if> msgin;
    sc_port<write_if> msgout;
    
    // module constructor
    SC_CTOR(Memory) {
        srandom(time(NULL));
        
        SC_THREAD(msgin_monitor);
    }
    
    void msgin_monitor() {
        while (true) {
            message_t msg;
            msgin->read(msg);
            if (msg.cmd == RD && msg.addr == DMEM) {
                wait(1 + random() % 10, SC_NS);
                message_t daval(DataValid, NOADDR, msg.tag);
                msgout->write(daval);
            }
            else if (msg.cmd == DataValid) {}
            
            else
                throw std::invalid_argument("in memory.cpp: Invalid messages from SSA: " + msg.toString());

        }
    }
};

#endif


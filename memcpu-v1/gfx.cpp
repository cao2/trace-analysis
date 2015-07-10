
#ifndef _GFX_CPP_
#define _GFX_CPP_

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
class GFX : public sc_module {
private:
    sc_event rdio;
    sc_event daval;
    
public:
    sc_port<read_if> req_in;
    sc_port<write_if> resp_out;
    sc_port<write_if> req_out;
    sc_port<read_if> resp_in;

    
    // module constructor
    SC_CTOR(GFX) {
        srandom(time(NULL));
        
        SC_THREAD(req_in_monitor);
        SC_THREAD(msi_gen);
        SC_THREAD(upstream_mem_write_gen);
        SC_THREAD(upstream_mem_read_gen);
    }
    
    void req_in_monitor() {
        while (true) {
            message_t msg;
            req_in->read(msg);
            if (msg.cmd == RD && msg.addr == MMIO) {
                wait(1 + random() % 2, SC_NS); // To allow interleaving with other threads.
                message_t msgout(DataValid, NOADDR, msg.tag);
                resp_out->write(msgout);
            }
            else
              throw std::invalid_argument("gfx.cc: Invalid request message from SSA");
        }
    }
    
    void msi_gen() {
        for (uint32_t i = 0; i < 10; i++) {
//while(true) {
            wait(1 + random() % 3, SC_NS);
            
            uint32_t intr_rdy = 1;//random() % 2;
            if (intr_rdy==1) {
                message_t reqout(WR, INTR, US_MSI_TAG);
                req_out->write(reqout);
            }
        }
    }
    
    // Flow: upstream memory write: initiate
    void upstream_mem_write_gen() {
        for (uint32_t i = 0; i < 10; i++) {
            wait(1 + random() % 3, SC_NS);
            message_t wr_mem(WR, DMEM, US_DMEM_WR_TAG);
            req_out->write(wr_mem);
        
            wait(1 + random() % 3, SC_NS);
            message_t daval(DataValid, NOADDR, US_DMEM_WR_TAG);
            resp_out->write(daval);
        }
    }
    
    // Flow: upstream memory read: initiate
    void upstream_mem_read_gen() {
        for (uint32_t i = 0; i < 10; i++) {
            wait(1 + random() % 3, SC_NS);
            message_t rd_mem(RD, DMEM, US_DMEM_RD_TAG);
            req_out->write(rd_mem);

            message_t resp;
            resp_in->read(resp);
            if (resp.cmd != DataValid) {
                throw std::invalid_argument("gfx.cc: Invalid request message from SSA");
            }
        }
    }
};

#endif


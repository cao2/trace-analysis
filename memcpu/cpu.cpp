
#ifndef _CPU_
#define _CPU_

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "comm_if.hpp"
#include "msg_def.hpp"

/*
 * A CPU generates some service requests to a memory.
 */
class CPU : public sc_module {
private:
    sc_event rdio;
    sc_event rdmem;
    sc_event wrmem;
    sc_event daval;
    sc_event msi;
    
public:
    sc_port<read_if> req_in;
    sc_port<write_if> resp_out;
    sc_port<read_if> resp_in;
    sc_port<write_if> req_out;
    
    SC_CTOR(CPU) {
        srandom(time(NULL));
        SC_THREAD(ds_mmio_rd_gen);
        SC_THREAD(ssa_req_in_rd_dmem_handler);
        SC_THREAD(ssa_req_in_wr_dmem_handler);
        SC_THREAD(ssa_req_in_msi_handler);
    }
    
    // Initiate a down stream MMIO read flow.
    void ds_mmio_rd_gen() {
        //while (true) {
        for (uint32_t i = 0; i < 10; i++) {
            wait(1 + random() % 10, SC_NS);
            
            // Initiate a downstream MMIO RD flow.
            message_t req;
            req.cmd = RD;
            req.addr = MMIO;
            req_out->write(req);
            
            // Wait for RD:MMIO request from SSA.
            message_t reqinmsg;
            req_in->read_cond(reqinmsg, RD, MMIO);

            // send message ReplyMiss back to SSA wrt MMIO RD request.
            message_t respout;
            respout.cmd = ReplyMiss;
            resp_out->write(respout);

            // wait for message DataValid from SSA.
            message_t respinmsg;
            resp_in->read(respinmsg);
            if (respinmsg.cmd != DataValid)
                throw std::invalid_argument("Invalid response message from SSA");
        }
    }
    
    // Process incoming messages
    void ssa_req_in_rd_dmem_handler() {
        while (true) {
            message_t reqin;
            req_in->read_cond(reqin, RD, DMEM);
    
            // Randomly signal cache hit or miss.
            uint32_t cachehit = random() % 2;
            message_t respout;
            if (cachehit==1) {
                respout.cmd = ReplyHit;
                resp_out->write(respout);
                respout.cmd = DataValid;
                resp_out->write(respout);
            }
            else {
                respout.cmd = ReplyMiss;
                resp_out->write(respout);
            }
        }
    }
    
    
    // Upstream Write to DMEM request
    void ssa_req_in_wr_dmem_handler() {
        while (1) {
            message_t reqin;
            req_in->read_cond(reqin, WR, DMEM);
    
            // Randomly signal cache hit or miss.
            uint32_t cachehit = random() % 2;
            message_t cpu_resp;
            if (cachehit==1) {
                cpu_resp.cmd = ReplyHit;
                resp_out->write(cpu_resp);
                wait(1 + random() % 3, SC_NS);
                cpu_resp.cmd = DataValid;
                resp_out->write(cpu_resp);
            }
            else {
                cpu_resp.cmd = ReplyMiss;
                resp_out->write(cpu_resp);
            }
        }
    }
    
    
    // GFX initiated MSI flow
    void ssa_req_in_msi_handler() {
        while (1) {
            message_t reqin;
            req_in->read_cond(reqin, MSI);
            message_t respout;
            respout.cmd = ReplyMiss;
            resp_out->write(respout);
        }
    }
};

#endif


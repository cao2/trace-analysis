/********************************************************************
 *  This version is a wrong implementation of the CPU model.
 *  
 *  It can be used to generated bad traces to exercise the
 *  trace analysis algorithm.
 ********************************************************************/


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
        SC_THREAD(ds_mmio_rd);
        SC_THREAD(req_in_handler);
    }
    
    // Initiate a down stream MMIO read flow.
    void ds_mmio_rd() {
        //while (true) {
        for (uint32_t i = 0; i < 10; i++) {
    
            wait(1 + random() % 10, SC_NS);
            message_t req;
            req.cmd = RD;
            req.addr = MMIO;
            req_out->write(req);
            //cerr << "CPU:SSA:" << req.toString() << endl;
            message_t respinmsg;
            resp_in->read(respinmsg);
            cerr << "read SSA:CPU:" << respinmsg.toString() << endl;
            if (respinmsg.cmd != DataValid)
                throw std::invalid_argument("Invalid response message from SSA");
        }
    }
    
    // Process incoming messages
    void req_in_handler() {
        while (true) {
            message_t reqin;
            req_in->read(reqin);
            
            if (reqin.cmd == RD) {
                if (reqin.addr == DMEM) {
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
                else if (reqin.addr == MMIO) {
                    message_t respout;
                    respout.cmd = ReplyMiss;
                    resp_out->write(respout);
                    //cerr << "CPU:SSA:" << respout.toString() << endl;
                }
                else
                    throw std::invalid_argument("Invalid memory address");
            }
            // Flow: Upstream Write to DMEM
            else if (reqin.cmd == WR && reqin.addr == DMEM) {
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
            // Flow: GFX initiated MSI
            else if (reqin.cmd == MSI) {
                message_t respout;
                respout.cmd = ReplyMiss;
                resp_out->write(respout);
            }
            else
                throw std::invalid_argument("Invalid request message from SSA");

        }
    }
};

#endif


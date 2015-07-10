#ifndef _BUSCTRL_
#define _BUSCTRL_

#include <stdexcept>
#include <assert.h>
#include "comm_if.hpp"


class BusCtrl : public sc_module {
private:
    enum {start, sync, d_rdy} mmiord_state;
    sc_event mmiord_sync_start;
    sc_event mmiord_sync_end;
    sc_event gfx_daval;
    
public:
    sc_port<read_if> cpu_req_in;
    sc_port<write_if> cpu_resp_out;
    sc_port<read_if> cpu_resp_in;
    sc_port<write_if> cpu_req_out;
    
    sc_port<read_if> mem_in;
    sc_port<write_if> mem_out;
    
    sc_port<write_if> gfx_req_out;
    sc_port<read_if> gfx_resp_in;
    sc_port<read_if> gfx_req_in;
    sc_port<write_if> gfx_resp_out;
    
    
    // module constructor
    SC_CTOR(BusCtrl) {
        srandom(time(NULL));
        
        mmiord_state = start;
        
        SC_THREAD(cpu_req_in_mmio_rd_handler);
        SC_THREAD(gfx_req_in_msi_handler);
        SC_THREAD(gfx_req_in_mem_wr_handler);
        SC_THREAD(gfx_req_in_mem_rd_handler);
    }
    
    void cpu_req_in_mmio_rd_handler() {
        while (true) {
            message_t cpureqin;
            message_t gfxresp;
            
            cpu_req_in->read(cpureqin);
            
            if (cpureqin.cmd == RD && cpureqin.addr == MMIO) {
                cpu_req_out->write(cpureqin);
                gfx_req_out->write(cpureqin);
                message_t cpurespin;
                cpu_resp_in->read_cond(cpurespin, ReplyMiss);
                cerr << "SSA:CPU -> mmio rd " << cpurespin.toString() << endl;
                message_t gfxresp;
                gfx_resp_in->read_cond(gfxresp, DataValid);
                cerr << "SSA:CPU -> mmio rd " << gfxresp.toString() << endl;
                assert(cpurespin.cmd == ReplyMiss && gfxresp.cmd == DataValid);
                cpu_resp_out->write(gfxresp);
                cerr << "SSA:CPU -> DataValid" << endl;
            }
            else
                throw std::invalid_argument("CPU: illegal message from CPU: " + cpureqin.toString());
        }
    }
    
    
    void gfx_req_in_msi_handler() {
        while (true) {
            message_t gfxreqin;
            gfx_req_in->read_cond(gfxreqin, WR, INTR);
            message_t req2cpu;
            req2cpu.cmd = MSI;
            //req2cpu.addr = INTR;
            cpu_req_out->write(req2cpu);
            message_t resp1cpu;
            cpu_resp_in->read_cond(resp1cpu, ReplyMiss);
        }
    }
    
    void gfx_req_in_mem_wr_handler() {
        while (true) {
            message_t gfxreqin;
            gfx_req_in->read_cond(gfxreqin, WR, DMEM);
            cpu_req_out->write(gfxreqin);
            message_t resp1cpu;
            message_t gfxrespin;
            gfx_resp_in->read_cond(gfxrespin, DataValid);
            cpu_resp_in->read_cond_select(resp1cpu, ReplyMiss, ReplyHit);
            if (resp1cpu.cmd == ReplyHit) {
                message_t data1cpu;
                cpu_resp_in->read_cond(data1cpu, DataValid);
            }
            message_t resp2mem;
            resp2mem.cmd = DataValid;
            assert(gfxrespin.cmd==DataValid);
            mem_out->write(resp2mem);
        }
    }
    
    void gfx_req_in_mem_rd_handler() {
        while (true) {
            message_t gfxreqin;
            gfx_req_in->read_cond(gfxreqin, RD, DMEM);
            cpu_req_out->write(gfxreqin);
            mem_out->write(gfxreqin);
            message_t resp1mem;
            mem_in->read_cond(resp1mem, DataValid);
            message_t resp1cpu;
            cpu_resp_in->read_cond_select(resp1cpu, ReplyMiss, ReplyHit);
            if (resp1cpu.cmd == ReplyHit) {
                message_t daval;
                cpu_resp_in->read_cond(daval, DataValid);
            }
            message_t wrd;
            wrd.cmd = DataValid;
            gfx_resp_out->write(wrd);
            mem_out->write(wrd);
        }
    }
};

#endif


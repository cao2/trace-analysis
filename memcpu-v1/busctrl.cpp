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
            uint32_t flow_tag = cpureqin.tag;
            
            if (cpureqin.cmd == RD && cpureqin.addr == MMIO) {
                cpu_req_out->write(cpureqin);
                gfx_req_out->write(cpureqin);
                message_t cpurespin(ReplyMiss, NOADDR, flow_tag);
                cpu_resp_in->read_cond(cpurespin);
                message_t gfxresp(DataValid);
                gfxresp.tag = flow_tag;
                gfx_resp_in->read_cond(gfxresp);
                assert(cpurespin.cmd == ReplyMiss && gfxresp.cmd == DataValid);
                cpu_resp_out->write(gfxresp);
            }
            else
                throw std::invalid_argument("CPU: illegal message from CPU: " + cpureqin.toString());
        }
    }
    
    
    void gfx_req_in_msi_handler() {
        while (true) {
            message_t gfxreqin(WR, INTR, NOTAG);
            gfx_req_in->read_cond(gfxreqin);
            
            message_t req2cpu(MSI, NOADDR, gfxreqin.tag);
            cpu_req_out->write(req2cpu);
            
            message_t resp1cpu(ReplyMiss, NOADDR, gfxreqin.tag);
            cpu_resp_in->read_cond(resp1cpu);
        }
    }
    
    void gfx_req_in_mem_wr_handler() {
        while (true) {
            message_t gfxreqin(WR, DMEM, NOTAG);
            gfx_req_in->read_cond(gfxreqin);
            cpu_req_out->write(gfxreqin);
            
            message_t gfxrespin(DataValid, NOADDR, gfxreqin.tag);
            gfx_resp_in->read_cond(gfxrespin);
            
            message_t resp1cpu;
            cpu_resp_in->read_cond_select(resp1cpu, ReplyMiss, ReplyHit, gfxreqin.tag);
            if (resp1cpu.cmd == ReplyHit) {
                message_t data1cpu(DataValid, NOADDR, gfxreqin.tag);
                cpu_resp_in->read_cond(data1cpu);
            }
            message_t resp2mem(DataValid, NOADDR, gfxreqin.tag);
            mem_out->write(resp2mem);
        }
    }
    
    void gfx_req_in_mem_rd_handler() {
        while (true) {
            message_t gfxreqin(RD, DMEM, NOTAG);
            gfx_req_in->read_cond(gfxreqin);
            
            cpu_req_out->write(gfxreqin);
            mem_out->write(gfxreqin);
            
            message_t resp1mem(DataValid, NOADDR, gfxreqin.tag);
            mem_in->read_cond(resp1mem);
            
            message_t resp1cpu;
            cpu_resp_in->read_cond_select(resp1cpu, ReplyMiss, ReplyHit, gfxreqin.tag);
            if (resp1cpu.cmd == ReplyHit) {
                message_t daval(DataValid, NOADDR, gfxreqin.tag);
                cpu_resp_in->read_cond(daval);
            }
            
            message_t wrd(DataValid, NOADDR, gfxreqin.tag);
            gfx_resp_out->write(wrd);
            mem_out->write(wrd);
        }
    }
};

#endif


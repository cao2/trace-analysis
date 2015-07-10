
#include "systemc.h"

#include "memory.cpp"
#include "cpu.cpp"
#include "queue.hpp"
#include "busctrl.cpp"
#include "gfx.cpp"

#include <iostream>

int sc_main(int argc, char *argv[]) {
    // provide a memory module for the simulation system
    Queue cpu2ctrl_req("cpu2ctrl_req");
    Queue ctrl2cpu_resp("ctrl2cpu_resp");
    Queue cpu2ctrl_resp("cpu2ctrl_resp");
    Queue ctrl2cpu_req("ctrl2cpu_req");
    Queue ctrl2mem("ctrl2mem");
    Queue mem2ctrl("mem2ctrl");
    
    Queue ctrl2gfx_req("ctrl2gfx_req");
    Queue gfx2ctrl_resp("gfx2ctrl_resp");
    Queue gfx2ctrl_req("gfx2ctrl_req");
    Queue ctrl2gfx_resp("ctrl2gfx_resp");
    

    CPU cpu0("CPU0");
    BusCtrl ctrl("SSA");
    Memory mem("DUNIT");
    GFX gfx("GFX");

    cpu0.req_in(ctrl2cpu_req);
    cpu0.resp_out(cpu2ctrl_resp);
    cpu0.resp_in(ctrl2cpu_resp);
    cpu0.req_out(cpu2ctrl_req);
    
    ctrl.cpu_req_in(cpu2ctrl_req);
    ctrl.cpu_resp_out(ctrl2cpu_resp);
    ctrl.cpu_resp_in(cpu2ctrl_resp);
    ctrl.cpu_req_out(ctrl2cpu_req);
    
    ctrl.mem_in(mem2ctrl);
    ctrl.mem_out(ctrl2mem);
    ctrl.gfx_resp_in(gfx2ctrl_resp);
    ctrl.gfx_req_out(ctrl2gfx_req);
    ctrl.gfx_req_in(gfx2ctrl_req);
    ctrl.gfx_resp_out(ctrl2gfx_resp);
    
    mem.msgin(ctrl2mem);
    mem.msgout(mem2ctrl);
    
    gfx.req_in(ctrl2gfx_req);
    gfx.resp_out(gfx2ctrl_resp);
    gfx.req_out(gfx2ctrl_req);
    gfx.resp_in(ctrl2gfx_resp);

    cpu2ctrl_req.setSrc("CPU0");
    cpu2ctrl_req.setDest("SSA");
    ctrl2cpu_resp.setSrc("SSA");
    ctrl2cpu_resp.setDest("CPU0");
    cpu2ctrl_resp.setSrc("CPU0");
    cpu2ctrl_resp.setDest("SSA");
    ctrl2cpu_req.setSrc("SSA");
    ctrl2cpu_req.setDest("CPU0");
    
    ctrl2mem.setSrc("SSA");
    ctrl2mem.setDest("DUNIT");
    mem2ctrl.setSrc("DUNIT");
    mem2ctrl.setDest("SSA");
    
    ctrl2gfx_req.setSrc("SSA");
    ctrl2gfx_req.setDest("GFX");
    gfx2ctrl_resp.setSrc("GFX");
    gfx2ctrl_resp.setDest("SSA");
    gfx2ctrl_req.setSrc("GFX");
    gfx2ctrl_req.setDest("SSA");
    ctrl2gfx_resp.setSrc("SSA");
    ctrl2gfx_resp.setDest("GFX");

    //* Start simulation for 10K NS.
    sc_start(100, SC_NS);
    //sc_start();
    
    return 0;
}


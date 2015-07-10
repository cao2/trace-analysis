
#include "systemc.h"

#include "memory.cpp"
#include "cpu.cpp"

#include "memadaptor_signals.cpp"
#include "cpuadaptor_signals.cpp"

#include "cpu_mem_bus.hpp"

#include <iostream>

int sc_main(int argc, char *argv[]) {
    // provide a memory module and a CPU for the simulation system
    Memory mem("MemoryModuleA");
    CPU cpu0("CPU0");

    // construct two adapters for "refining" cpu/mem communication
    MemAdaptor_Signals memadapt("MemAdaptA");
    CPUAdaptor_Signals cpuadapt("CPUAdapt0");

    // a set of signals (CPU_Mem_Bus) is used to connect the adaptors' ports
    CPU_Mem_Bus cpumembus;
    cpumembus.reset();

    // binding bus ports to the cpu adaptor
    cpuadapt.addr(cpumembus.addr);
    cpuadapt.data(cpumembus.data);
    cpuadapt.memsize(cpumembus.memsize);
    cpuadapt.r_w(cpumembus.r_w);
    cpuadapt.req(cpumembus.req);
    cpuadapt.ack(cpumembus.ack);
    cpuadapt.err(cpumembus.err);

    // binding bus ports to the memory adaptor
    memadapt.addr(cpumembus.addr);
    memadapt.data(cpumembus.data);
    memadapt.memsize(cpumembus.memsize);
    memadapt.r_w(cpumembus.r_w);
    memadapt.req(cpumembus.req);
    memadapt.ack(cpumembus.ack);
    memadapt.err(cpumembus.err);

    // binding the memory adaptor the actual memory module
    memadapt.memory(mem);
    // binding the cpu adaptor to the cpu's memory port
    cpu0.memory(cpuadapt);

    // also try with two CPUs (output messages jams may happen)
    //CPU cpu1("CPU1");
    //cpu1.memory(mem);

    // starting the simulation
    sc_start();
}


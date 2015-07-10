
#include "systemc.h"

#include "producer.cpp"
#include "consumer.cpp"

int sc_main(int argc, char *argv[]) {
    // instantiate a producer and a consumer
    Producer tr("Transmitter1");
    Consumer rc("Receiver1");

    // instantiate a system clock: period 10 ms, duty cycle 50%
    sc_clock clk("Clock1", 10, SC_MS, 0.5);
    tr.clk(clk);

    // instantiate a wire to bind both modules to
    sc_signal<int> linktrrc;
    tr.value(linktrrc);
    rc.value(linktrrc);

    // start a 50 ms simulation of this system
    sc_start(50, SC_MS);
}


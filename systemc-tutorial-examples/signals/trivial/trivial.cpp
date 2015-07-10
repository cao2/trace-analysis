#include "systemc.h"

// a signal transmitter
class Transmitter : public sc_module {
    public:
        sc_out<bool> sigout;
        SC_CTOR(Transmitter) { /* do nothing */ }
};

// a signal receiver
class Receiver : public sc_module {
    public:
        sc_in<bool> sigin;
        SC_CTOR(Receiver) { /* do nothing */ }
};

// main function for preparing the system to verify,
// and run the simulation
int sc_main(int argc, char *argv[]) {
    // instantiate a Transmitter
    Transmitter tr("Transmitter1");
    
    // instantiate two Receivers
    Receiver rc1("Receiver1");
    Receiver rc2("Receiver2");

    // instantiate a wire to join their ports
    sc_signal<bool> linktrrc;

    // link the wire to each modules' port
    tr.sigout(linktrrc);
    rc1.sigin(linktrrc);
    rc2.sigin(linktrrc);

    // start a do-nothing simulation
    sc_start();
}


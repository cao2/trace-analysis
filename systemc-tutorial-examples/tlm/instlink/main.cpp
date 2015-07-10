
#include "systemc.h"

#include "dumbfooimplementor.cpp"
#include "dumbfooexploiter.cpp"

/*
 * This function is called by the SystemC simulation
 * kernel. It's expected to:
 *   @ instantiate modules
 *   @ link them and possibly provide clocks
 *   @ start the simulation with the desired parameters
 */
int sc_main(int argc, char *argv[]) {
    // intantiating a service provider module
    DumbFooImplementor srv("ServiceProvider1");
    // intantiating a service provider module
    DumbFooExploiter usr("ServiceExploiter1");

    // registering the service provider to the exploiter
    usr.foo(srv);

    // starting the simulation
    sc_start();
}



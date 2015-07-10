#include <iostream>

#include "systemc.h"

// the foo_if interface declaration has to be imported
#include "fooif.hpp"

/*
 * This instantiable module provides a do-nothing implementation
 * of the foo_if interface.
 */
class DumbFooImplementor : public sc_channel, public foo_if {
    public:
        // constructor, empty but mandatory
        SC_CTOR(DumbFooImplementor) { }

        // dump implementation of the function "doit()", required
        // by the interface
        void doit(int param) {
            // do useless things, with param
            cout << "Got: " << param << endl;
        }
    
        // more module-specific functions can be added to extend the
        // basic features required by foo_if
        // ...
};


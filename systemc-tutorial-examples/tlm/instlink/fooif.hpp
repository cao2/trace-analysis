

#ifndef _FOOIF_HPP_
#define _FOOIF_HPP_

#include "systemc.h"

/*
 * foo_if is a module interface, because it extends sc_interface.
 * Unlike modules, interfaces cannot be instantiated on their own.
 * Modules applying to this interface must implement all of its
 * methods to become instantiable.
 *
 * The foo_if interface requires a single method "doit()". More
 * methods can be inserted in general.
 */
class foo_if : public sc_interface {
    public:
        // the list of services required by this interface
        
        /*
         * this method does what it does
         */
        virtual void doit(int param) = 0;
};

#endif


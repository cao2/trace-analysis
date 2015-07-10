
########################
        TRIVIAL
########################

last update aug 17, 2006
report bugs to <mij@bitchx.it>


    @ Outline @

This example outlines one of the simplest kinds of simulation for SystemC.

Two modules are specified: a transmitter (Transmitter) and a receiver
(Receiver). While no behaviour is specified for any of them, they feature
one port each, that's meant for sending and receiving signals.

The sc_main() function instantiates one instance of a Transmitter module,
and two instances of a Receiver. Then, a further "signal" (wire) is
instantiated for binding these ports together.


    @ Goals @

This example provides a more consistent introduction to:
- how to define modules
- input/output ports
- how to instantiate modules, how to use module names
- signals & how to bind module ports


    @ Compile & Run @

$ make
$ ./trivial

The expected output is one of an empty SystemC simulation:

$ ./buffer 

             SystemC 2.1.v1 --- Aug 16 2006 18:19:38
        Copyright (c) 1996-2005 by all Contributors
                    ALL RIGHTS RESERVED
$


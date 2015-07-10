
########################
        BUFFER
########################

last update aug 14, 2006
report bugs to <mij@bitchx.it>


    @ Outline @

A module (Bufwriter) issues write and read operations against a buffer it is
connected to, continuously and randomly.  The actual buffer is a module
conforming to a specific interface (buffer_if).

Two implementations are given for such interface: a simpler one, with single
room for storing data (FlatBuffer) and one with multiple slots, FIFO-managed
(ComplexBuffer).

Since both conform to the interface expected by the active module, the can
both be connected to it.


    @ Goals @

This example provides a more consistent introduction to:
- interface/implementation relationship
- role of interfaces
- defining initialization and activity of thicker modules
- advanced use of the TLM paradigm, with exceptions


    @ Compile & Run @

$ make
$ ./buffer

The expected output is a (endless) list of operation result reports, like
this:

$ ./buffer 

             SystemC 2.1.v1 --- Aug 16 2006 18:19:38
        Copyright (c) 1996-2005 by all Contributors
                    ALL RIGHTS RESERVED
Write OK.
Read OK.
Buffer was empty.
Write OK.
Write OK.
Buffer was busy.
...

You can safely terminate the simulation by pressing Ctrl-C.



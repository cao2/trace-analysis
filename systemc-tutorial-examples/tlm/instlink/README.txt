
########################
       INSTLINK 
########################

last update aug 12, 2006
report bugs to <mij@bitchx.it>


    @ Outline @

Two dumb modules interact through a trivial interface "foo_if": a first module
provides services complying with this interface (DumbFooImplementor) and a
second one exploits them (DumbFooExploiter).
Four source files show how to organize, declare, and use these components.
The main source file in particular shows how to import, instantiate and link
modules.


    @ Goals @

This example provides a first introduction to:
- what are interfaces, how they are declared and 
- how to define simple modules (active and passive ones) and their activity
- how to instantiate and link modules
- how to run the simulation


    @ Compile & Run @

$ make
$ ./instlink

The expected output is a message from the implementor:

$ ./instlink

             SystemC 2.1.v1 --- Aug  1 2006 13:33:13
        Copyright (c) 1996-2005 by all Contributors
                    ALL RIGHTS RESERVED
Got: 123
$


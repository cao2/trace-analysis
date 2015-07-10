
########################
        MEMCPU
########################

last update aug 18, 2006
report bugs to <mij@bitchx.it>


    @ Outline @

A simulation is performed on a system featuring one memory and one cpu.
The memory is pure slave, the CPU pure master.

A memory interface (mem_if) dictates what services complying memories must
feature; this is a rich interface offering both unit and block operations,
and an extra service for inspecting memory features (specifically, the
capacity of the module). A notable feature of the mem_if interface is that it
rules both the services and the data types implementations must employ.

The CPU implementation demonstrates all the supported operations with the
memory. It first gets to know what capacity the memory offers. Then, it picks
a random (but valid) memory address to operate at. Finally, it issues a write
and read operation with a data unit, and repeats the same with the more
advanced block operations provided by the memory.

The given memory implementation (Memory) of the mem_if interface is a usual
one, with an array of data units. This array is didactically instantiated and
deleted dynamically when the module is constructed and deleted, respectively.

At the function-call stratum, it may be noticed the use of exception throws
for expressing error conditions. This brings convenient clarity when, at a
lower layer, the actual communication between the modules will be formalized.


    @ Goals @

This example mainly focuses on module interfaces. A more structured example
which shows data types and a larger set of services is provided.


    @ Compile & Run @

$ make
$ ./memcpu

The expected output is a list of service-issue messages from the CPU:

$ ./memcpu 

             SystemC 2.1.v1 --- Aug  1 2006 13:33:13
        Copyright (c) 1996-2005 by all Contributors
                    ALL RIGHTS RESERVED
CPU: will write T into memory's 555044 cell.
CPU: memory location 555044 now holding 'T'.
CPU: will write: T1C;G;Z80M
CPU: getting back 5 units of data: T1C;G
$


*** The trace produced by this version causes problem during analysis. 

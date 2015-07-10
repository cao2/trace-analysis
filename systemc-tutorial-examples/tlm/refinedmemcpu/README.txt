
#################################
        refined MEMCPU
#################################

last update aug 17, 2006
report bugs to <mij@bitchx.it>


    @ Outline @

This example is an extension and deepening of a previous example -- memcpu.
The explanation of the inherited part is available in that module's
documentation.

Refined MEMCPU is a refinement of memcpu, in which the actual communication
between the CPU and Memory modules is modelled. This improvement is
accomplished transparently with respect of both the CPU and the Memory modules
themselves, by taking advantage of the TLM model.

Two additional modules are introduced in the system for implementing such
communication. To appear transparent to the rest of the system (CPU and
Memory instances) they masquerade the communication complexity under the
pattern of adapters. A cpu adapter and a memory adapter interface respectively
to CPU and Memory modules in the form of intermediaries. So, the cpuadapter
must implement the memory module interface, and the memory adaptor must behave
to the actual memory like the CPU module does:

     +-------+                           +--------+
     |       |  +----+           +----+  |        |
     |  cpu  O--| CA |@@@@@@@@@@@| MA O--| memory |
     |       |  +----+           +----+  |        |
     +-------+                           +--------+

The actual, hardware implementation is specified through the usual signals
paradigm. More specifically, CPUAdaptor (shortly, CA) and MemAdaptor (MA)
communicate through a set of signals wrapped to realize a full-featured bus.
For cleaness of implementation, but not for constraint, such signals are
wrapped into a structure on its own (cpu_mem_bus) that will be connected to
each adapter port when system modules are instantiated.

A protocol must be specified for the communication to be accomplished across
the bus. This is entirely enclosed into adapters implementations and can be
changed in the future without impact against the rest of the system relying on
them.

Besides system initialization (sc_main body), nothing is changed throughout
the system for the introduction of the adapters.

What it is important to notice after experiencing this example is:
- what adaptors are: they translate the TLM service model into a real,
  hardware way of communicating
- what changes for this deeper specification: introduction of adapters and
  linking of modules
- how modules are now connected: CPU --> cpu_adaptor <--> mem_adaptor --> memory
- CPUAdaptor implements the mem_if memory interface to be plugged into CPU's
  memory port
- despite CPU, CPUAdaptor is not active at all. It fires actions in response
  to requests of service by the CPU itself
- MemAdaptor features a mem_if port to be linked to the actual module

Note: for simplicity and not for lack of capability, the additional complexity
of modelling a time-discrete (clocked) system has been withdrawn with respect
to memcpu's original example.


    @ Goals @

This example introduces the approach of system specification refinement with a
fairly simple, yet large and complete solution.


    @ Compile & Run @

$ make
$ ./refinedmemcpu

The expected output is a list of service-issue messages from the CPU,
interleaved with messages from the memory adaptor responding to the bus
protocol.

$ ./refinedmemcpu 
             SystemC 2.1.v1 --- Aug 16 2006 18:19:38
        Copyright (c) 1996-2005 by all Contributors
                    ALL RIGHTS RESERVED
MemAdaptor: waiting for requests...
MemAdaptor: SIZE request income, processing... done.
CPU: size request served, capacity is 1000000 data units.
CPU: will write E into memory's 150715 cell.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: READ request income, processing... done.
CPU: memory location 150715 now holding 'E'.
CPU: will write: EWPCJBP92K
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: WRITE request income, processing... done.
CPU: getting back 5 units of data...MemAdaptor: waiting for requests...
MemAdaptor: READ request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: READ request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: READ request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: READ request income, processing... done.
MemAdaptor: waiting for requests...
MemAdaptor: READ request income, processing... done.
CPU: got back EWPCJ
MemAdaptor: waiting for requests...
$

In case of uncomfortable terminals, this long output might be redirected into
a file for easier inspection.


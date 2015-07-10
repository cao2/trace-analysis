#################################
        SystemC examples
#################################

last update august 14, 2006

Requirements:
- c++ compiler with the Standard Template Library
- make
- SystemC v. 2.1 available somewhere in the system

To enable the examples to be compiled, the SystemC paths must be specified.
Run the syscpath.sh utility and look at its help message to accomplish this.

$ ./syscpath.sh


All the examples are:
- documented by an individual README.txt file
- ready to compile with the make command
- runnable with ./exampname

Spend the tour in this order:

1. SIGNALS
    1.1 trivial
    1.2 prodcons

2. TLM (Transaction-Level Modelling)
    2.1 instlink
    2.2 buffer
    2.3 memcpu
    2.4 refinedmemcpu



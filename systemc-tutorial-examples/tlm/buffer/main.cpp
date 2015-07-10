
#include "systemc.h"

#include "flatbuffer.cpp"
#include "complexbuffer.cpp"
#include "bufwriter.cpp"

/* simulation main function */
int sc_main(int argc, char *argv[]) {
    // instantiate a FlatBuffer and a Bufwriter
    FlatBuffer buf1("Buffer1");
    Bufwriter writer("BufWriter1");

    /* alternative: instantiate a ComplexBuffer */
    ComplexBuffer buf2("Buffer2");
    
    // link the buffer to the writer
    writer.buf(buf2);
    // also try:
    //writer.buf(buf2);     // feeding writer the complex buffer

    // start the simulation
    sc_start();
}


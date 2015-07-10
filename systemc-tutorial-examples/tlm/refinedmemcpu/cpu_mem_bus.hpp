
#ifndef _CPU_MEM_BUS_
#define _CPU_MEM_BUS_

/*
 * CPU_Mem_Bus structure wraps the "wires" exploited by
 * the cpu-mem adaptors for communicating.
 */
typedef struct _CPU_Mem_Bus {
    public:
        sc_signal<address_t> addr;
        sc_signal<char> data;
        sc_signal<bool> memsize; // get size request
        sc_signal<bool> r_w; // T: read / F: write
        sc_signal<bool> req;
        sc_signal<bool> ack;
        sc_signal<bool> err;
        void reset() {
            req.write(false);
            ack.write(false);
            err.write(false);
        }
} CPU_Mem_Bus;

#endif


#ifndef _MSG_DEF_
#define _MSG_DEF_

#include <strstream>
#include <string>

#include "cmd_def.hpp"

// Abstractions of address space partitions.
typedef uint32_t address_t;

const uint32_t NDEF = 0;
const uint32_t INTR = 1;
const uint32_t DMEM = 2;
const uint32_t MMIO = 3;

string addr_vector[4] = {"-", "INTR", "DMEM", "MMIO"};

string getAddr(address_t addr) {
    return addr_vector[addr];
}

class message_t
{
public:
    command_t   cmd;
    uint16_t    src_id;
    uint16_t    dest_id;
    uint32_t    addr;
    uint32_t    data;
    
    message_t() {
        cmd = NOCMD;
        addr = NDEF;
    };
    
    message_t(const message_t& other) {
        cmd = other.cmd;
        src_id = other.src_id;
        dest_id = other.dest_id;
        addr = other.addr;
        data = other.data;
    }
    
    message_t& operator=(const message_t& other) {
        cmd = other.cmd;
        src_id = other.src_id;
        dest_id = other.dest_id;
        addr = other.addr;
        data = other.data;
        return *this;
    }
    
    bool operator==(const message_t& other) {
        return (cmd == other.cmd &&
                src_id == other.src_id &&
                dest_id == other.dest_id &&
                addr == other.addr &&
                data == other.data);
    }
    
    void clear() {
        cmd = NOCMD;
        addr = NDEF;
    }
    
    std::string toString(void) {
        return getCmd(cmd) + ":" + getAddr(addr);
    }
};

#endif


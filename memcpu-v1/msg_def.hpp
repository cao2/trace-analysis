#ifndef _MSG_DEF_
#define _MSG_DEF_

#include <strstream>
#include <string>

#include "sys-def.h"
#include "cmd_def.hpp"

// Abstractions of address space partitions.
typedef uint32_t address_t;

const uint32_t NOADDR = 0;
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
    uint32_t    addr;
    uint32_t    data;
    uint32_t    tag;
    
    message_t() {
        cmd = NOCMD;
        addr = NOADDR;
        tag = NOTAG;
    };

    message_t(command_t command) {
        cmd = command;
        addr = NOADDR;
        tag = NOTAG;
    };
    
    message_t(command_t command, address_t address) {
        cmd = command;
        addr = address;
        tag = NOTAG;
    };
    
    message_t(command_t command, address_t address, uint32_t msg_tag) {
        cmd = command;
        addr = address;
        tag = msg_tag;
    };
    
    message_t(const message_t& other) {
        cmd = other.cmd;
        addr = other.addr;
        data = other.data;
        tag = other.tag;
    }
    
    message_t& operator=(const message_t& other) {
        cmd = other.cmd;
        addr = other.addr;
        data = other.data;
        tag = other.tag;
        return *this;
    }
    
    bool operator==(const message_t& other) {
        return (cmd == other.cmd &&
                addr == other.addr &&
                data == other.data &&
                tag == other.tag);
    }
    
    void clear() {
        cmd = NOCMD;
        addr = NOADDR;
        tag = NOTAG;
    }
    
    std::string toString(void) {
        return getCmd(cmd) + ":" + getAddr(addr);
    }
};

#endif


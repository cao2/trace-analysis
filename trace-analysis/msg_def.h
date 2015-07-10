#ifndef _MSG_H_
#define _MSG_H_

#include <strstream>
#include <string>

using namespace std;

typedef uint32_t config_t;
const config_t null_cfg = 0;

// Define indices for components.
const uint16_t CPU = 1;
const uint16_t SSA = 2;
const uint16_t GFX = 3;
const uint16_t DUNIT = 4;
string blk_vector[5] = {"-", "CPU", "SSA", "GFX", "DUNIT"};

// Define commands
typedef uint16_t command_t;
const command_t NOCMD = 0;
const command_t RD = 1;
const command_t WR = 2;
const command_t DataValid = 3;
const command_t ReplyHit = 4;
const command_t ReplyMiss = 5;
const command_t MSI = 6;
string cmd_vector[7] = {"-", "RD", "WR", "DataValid", "ReplyHit", "ReplyMiss", "MSI"};

// Define the ranges of memory address spaces.
typedef uint32_t address_t;
const uint32_t NDEF = 0;
const uint32_t INTR = 1;
const uint32_t DMEM = 2;
const uint32_t MMIO = 3;
string addr_vector[4] = {"-", "INTR", "DMEM", "MMIO"};


class message_t
{
public:
    uint32_t src;
    uint32_t dest;
    command_t cmd;
    address_t addr;
    
    config_t pre_cfg, post_cfg;
    
    message_t() {
        src = dest = cmd = addr = NDEF;
        pre_cfg = post_cfg = null_cfg;
    };
    
    message_t(const message_t& other) {
        src = other.src;
        dest = other.dest;
        cmd = other.cmd;
        addr = other.addr;
        pre_cfg = other.pre_cfg;
        post_cfg = other.post_cfg;
    }
    
    message_t& operator=(const message_t& other) {
        src = other.src;
        dest = other.dest;
        cmd = other.cmd;
        addr = other.addr;
        pre_cfg = other.pre_cfg;
        post_cfg = other.post_cfg;
        return *this;
    }
    
    bool operator==(const message_t& other) {
        return (src == other.src &&
                dest == other.dest &&
                cmd == other.cmd &&
                addr == other.addr &&
                pre_cfg == other.pre_cfg &&
                post_cfg == other.post_cfg);
    }
    
    void insert_pre_cfg(uint32_t i) {
        pre_cfg = pre_cfg | (1 << i);
    }
    
    void insert_post_cfg(uint32_t i) {
        post_cfg = post_cfg | (1 << i);
    }
    
    void set_msg(uint16_t source, uint16_t destination, command_t command, address_t address) {
        src = source;
        dest = destination;
        cmd = command;
        addr = address;
    }
    
    std::string toString(void) const
    {
        return blk_vector[src] + ":" +  blk_vector[dest] + ":" + cmd_vector[cmd] + ":" + addr_vector[addr];
    }
};

#endif


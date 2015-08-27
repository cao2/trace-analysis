#ifndef _MSG_H_
#define _MSG_H_

#include <strstream>
#include <string>

using namespace std;

typedef uint64_t config_t;
const config_t null_cfg = 0;

// Define indices for components.
const uint16_t cpu0 = 1;
const uint16_t icache0 = 2;
const uint16_t cpu1 = 3;
const uint16_t icache1 = 4;
const uint16_t membus = 5;
const uint16_t mem = 6;
const uint16_t dcache0=7;
const uint16_t dcache1=8;
string blk_vector[9] = {"-", "cpu0", "icache0", "cpu1", "icache1", "membus","mem","dcache0","dcache1"};

// Define commands
typedef uint16_t command_t;
const command_t readreq = 1;
const command_t readres = 2;
const command_t writereq = 3;
const command_t writeres = 4;
const command_t readExreq = 5;
const command_t readExres = 6;
const command_t storeCondreq = 7;
const command_t storeCondres = 8;
const command_t Upgradereq = 9;
const command_t Upgraderes = 10;
const command_t loadLockedreq=11;
const command_t loadLockedres=12;
const command_t StoreCondFailreq=13;
const command_t read=14;
const command_t write=15;
const command_t iFunc=16;
const command_t loadLocked=17;
const command_t Upgrade=18;

string cmd_vector[19] = {"-", "readreq", "readres", "writereq", "writeres", "readExreq", "readExres","storeCondreq","storeCondres","Upgraderes","Upgradereq","loadLockedreq","loadLockedres","StoreCondFailreq","read","write","iFunc","loadLocked","Upgrade"};

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

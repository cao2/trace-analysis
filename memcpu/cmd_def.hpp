#ifndef _CMD_DEF_
#define _CMD_DEF_

#include <stdexcept>
#include <iostream>

using namespace std;

typedef uint16_t command_t;

const command_t NOCMD = 0;
const command_t RD = 1;
const command_t WR = 2;
const command_t DataValid = 3;
const command_t ReplyHit = 4;
const command_t ReplyMiss = 5;
const command_t MSI = 6;


string cmd_vector[7] = {"-", "RD", "WR", "DataValid", "ReplyHit", "ReplyMiss", "MSI"};


string getCmd(command_t cmd)
{
    if (cmd > 7) {
        throw std::invalid_argument("Invalid command");
    }
    return cmd_vector[cmd];
}

#endif


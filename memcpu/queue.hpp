//
//  queue.hpp
//  
//
//  Created by zheng on 8/30/14.
//
//

#ifndef _queue_hpp
#define _queue_hpp

//#define PRINT_SEND

#include "comm_if.hpp"
#include "msg_def.hpp"


const uint32_t capacity = 10;

class Queue : public sc_channel, public write_if, public read_if
{
private:
    message_t* data;
    uint32_t first;
    uint32_t size;
    sc_event we, re;
    
    string src, dest;
    
public:
    Queue(sc_module_name name) : sc_channel(name) {
        first = 0;
        size = 0;
        data = new message_t[capacity];
    }
    
    ~Queue() {
        delete data;
    }
    
    void write(message_t& msg) {
        if (size == capacity) {
            wait(re);
        }
        
        data[(first+size)%capacity] = msg;
        size++;
#ifdef PRINT_SEND
        cout << src << ":" << dest << ":" << msg.toString() << endl << flush;
#endif
        we.notify();
    }
    
    
    void read(message_t& result) {
        if (size == 0) {
            wait(we);
        }
        
        result = data[first];
        first = (first+1) % capacity;
        size--;
#ifndef PRINT_SEND
        cout << src << ":" << dest << ":" << result.toString() << endl << flush;
#endif
        re.notify();
    }

    void read_cond(message_t& result, command_t good_cmd) {
    start:
        if (size == 0)
            wait(we);

        if (size == 0) {
            goto start;
        }
        
        assert(size > 0);
        message_t head = data[first];
        if (head.cmd != good_cmd) {
            wait(re);
            goto start;
        }
        assert(size > 0);

        result = data[first];
        first = (first+1) % capacity;
        size--;
#ifndef PRINT_SEND
        cout << src << ":" << dest << ":" << result.toString() << endl << flush;
#endif
        re.notify();
    }


    void read_cond(message_t& result, command_t good_cmd, address_t good_addr) {
    start:
        if (size == 0)
            wait(we);
        
        if (size == 0) {
            goto start;
        }
        assert(size > 0);

        message_t head = data[first];
        if (head.cmd != good_cmd || head.addr != good_addr) {
            wait(re);
            goto start;
        }
        
        assert(size > 0);
        result = data[first];
        first = (first+1) % capacity;
        size--;
#ifndef PRINT_SEND
        cout << src << ":" << dest << ":" << result.toString() << endl << flush;
#endif
        re.notify();
    }
    
    // Read the message if its command is either cmd1 or cmd2
    void read_cond_select(message_t& result, command_t cmd1, command_t cmd2) {
    start:
        if (size == 0)
            wait(we);
        
        if (size == 0) {
            goto start;
        }
        assert(size > 0);

        message_t head = data[first];
        if (head.cmd != cmd1 && head.cmd != cmd2) {
            wait(re);
            goto start;
        }
        assert(size > 0);

        result = data[first];
        first = (first+1) % capacity;
        size--;
#ifndef PRINT_SEND
        cout << src << ":" << dest << ":" << result.toString() << endl << flush;
#endif
        re.notify();
    }
    
    void setSrc(const string& s) {
        src = s;
    }
    
    void setDest(const string& d) {
        dest = d;
    }

};

#endif

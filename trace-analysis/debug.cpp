//
//  debug.cpp
//  
//
//  Created by Yuting Cao on 8/17/15.
//
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include "lpn.h"
#include <set>
#include <algorithm>    // std::sort
#include <math.h>
#include "debug.h"
struct omsg{
    string cmd;
    string iid;
    omsg(){
        cmd=iid="none";
    }
};
int main(int argc, char *argv[]){
    
    
    ifstream trace_file(argv[1]);
    if (trace_file.is_open()) {
        
        std::string line;
        omsg new_msg;
        while (getline(trace_file, line)){
            // From each line, get the message information to create a new msg.
            
            uint32_t p1, p2,  p3,p4,p5,p6,p7;
            p1=p2=p3=p4=p5=p6=p7=0;
            for (uint32_t i = 0; i < line.size(); i++)
                if (line.at(i) == ':') {
                    p1 = i;
                    break;
                }
            for (uint32_t i = p1+1; i < line.size(); i++)
                if (line.at(i) == ':') {
                    p2 = i;
                    break;
                }
            for (uint32_t i = p2+1; i < line.size(); i++)
                if (line.at(i) == ':') {
                    p3 = i;
                    break;
                }
            for (uint32_t i = p3+1; i < line.size(); i++)
                if (line.at(i) == ':') {
                    p4 = i;
                    break;
                }
            for (uint32_t i = p4+1; i < line.size(); i++)
                if (line.at(i) == ':') {
                    p5 = i;
                    break;
                }
            for (uint32_t i = p5+1; i < line.size(); i++)
                if (line.at(i) == ':') {
                    p6 = i;
                    break;
                }
            for (uint32_t i = p6+1; i < line.size(); i++)
                if (line.at(i) == ':') {
                    p7 = i;
                    break;
                }
            new_msg.cmd = line.substr(p6+1, p7-p6-1);
            new_msg.iid= line.substr(p7);
    }
    return 1;
}

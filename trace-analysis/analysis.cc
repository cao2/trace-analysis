
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include "lpn.h"

// build a lpn model for each flow.
lpn_t* build_msi_flow_v1(void);
lpn_t* build_cpu0_read(void);
lpn_t* build_cpu1_read(void);
lpn_t* build_us_mem_rd_flow_v1(void);

struct flow_instance_t
{
    lpn_t* flow_inst;
    config_t cfg;
    
    flow_instance_t() {
        flow_inst = nullptr;
        cfg = null_cfg;
    }
    
    flow_instance_t(const flow_instance_t& other) {
        flow_inst = other.flow_inst;
        cfg = other.cfg;
    }
    
    bool operator==(const flow_instance_t& other) {
        bool rst=(flow_inst->get_flow_name() == other.flow_inst->get_flow_name() &&cfg == other.cfg);
        //cout<<"name compare    "<<flow_inst->get_flow_name()<<"::"<<other.flow_inst->get_flow_name()<<endl;
        //cout<<"cfg compare   "<<cfg<<"::"<<other.cfg<<endl;
        //cout<<"resutl"<<rst<<endl;
        return rst;
    }
    
    flow_instance_t& operator=(const flow_instance_t& other) {
        flow_inst = other.flow_inst;
        cfg = other.cfg;
        return *this;
    }
    
};

typedef vector<flow_instance_t> scenario_t;

void print_scenario(const scenario_t& scen)
{
    for (uint32_t i = 0; i < scen.size(); i++) {
        const flow_instance_t& f = scen.at(i);
        
        uint32_t cfg = f.cfg;
        string cfg_str;
        bool cfg_convert_begin = true;
        for (uint32_t i = 0; i < 32; i++) {
            if ((cfg & 1) == 1 ) {
                if (cfg_convert_begin) {
                    cfg_str = to_string(i);
                    cfg_convert_begin = false;
                }
                else
                    cfg_str += " " + to_string(i);
            }
            cfg = cfg >> 1;
        }
        cfg_str = "<" + cfg_str + ">";
        
        cout << "(" << f.flow_inst->get_flow_name() << "  " << cfg_str << ")  ";
    }
    cout << endl;
}


void print_scenario(const vector<lpn_t*> flow_spec, const scenario_t& scen)
{
    vector<uint32_t> flow_inst_cnt;
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    
    for (uint32_t i = 0; i < scen.size(); i++) {
        const flow_instance_t& f = scen.at(i);
        uint32_t cfg = f.cfg;
        string cfg_str;
        bool cfg_convert_begin = true;
        for (uint32_t i = 0; i < 32; i++) {
            if ((cfg & 1) == 1 ) {
                if (cfg_convert_begin) {
                    cfg_str = to_string(i);
                    cfg_convert_begin = false;
                }
                else
                    cfg_str += " " + to_string(i);
            }
            cfg = cfg >> 1;
        }
        cfg_str = "<" + cfg_str + ">";
        uint32_t flow_index = f.flow_inst->get_index();
        ++flow_inst_cnt.at(flow_index);
        cout << "\t(" << f.flow_inst->get_flow_name() << " \t " << cfg_str<< ")\t";
        cout << endl;
    }
    
    cout << "***  # of flow instances:" << endl;
    for (uint32_t i = 0; i < flow_inst_cnt.size(); i++) {
        lpn_t* flow = flow_spec.at(i);
        cout << "\t" << flow->get_flow_name() << ": \t" << flow_inst_cnt.at(flow->get_index()) << endl;
    }
    
    cout << endl;
}

bool equalscen(scenario_t x, scenario_t y){
    bool rst=true;
    if(x.size()==y.size())
    {
        for(uint32_t i=0;i<x.size();i++){
            if(x.at(i)==y.at(i));
            else
                rst=false;
        }
    }
    else
        rst=false;
    return rst;
}
vector<scenario_t> dscen(vector<scenario_t> vec){
    vector<scenario_t> rst;
    rst.push_back(vec.at(0));
    for(uint32_t i=0;i<vec.size();i++){
        bool flag=true;
        for(uint32_t j=0; j< rst.size(); j++){
            if(equalscen(vec.at(i),rst.at(j))!=true)
                flag=false;
        }
        if (flag==false)
            rst.push_back(vec.at(i));
    }
    return rst;
}
int main(int argc, char *argv[]) {
    
    // Build flow specification
    vector<lpn_t*> flow_spec;
    lpn_t* msi_flow = build_msi_flow_v1();
    lpn_t* cpu0_read = build_cpu0_read();
    lpn_t* us_mem_wr=build_us_mem_rd_flow_v1();
    lpn_t* cpu1_read=build_cpu1_read();
    flow_spec.push_back(msi_flow);
    msi_flow->set_index(0);
    flow_spec.push_back(cpu0_read);
    cpu0_read->set_index(1);
    flow_spec.push_back(us_mem_wr);
    us_mem_wr->set_index(2);
    flow_spec.push_back(cpu1_read);
    cpu1_read->set_index(3);
    
    
    vector<uint32_t> flow_inst_cnt;
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    
    
    
    vector<message_t> trace;
    
    // Open input trace file
    ifstream trace_file(argv[1]);
    if (trace_file.is_open()) {
        std::string line;
        message_t new_msg;
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
            
            cout<<line<<"  "<<p1<<"  "<<p2<<"  "<<p3<<endl;
            string tmp_str = line.substr(p3+1, p4-p3-1);

            if (tmp_str == "cpu0")
                new_msg.src = cpu0;
            else if (tmp_str == "dcache0")
                new_msg.src = cache0;
            else if (tmp_str == "icache0")
                new_msg.src = cache0;
            else if (tmp_str == "cpu1")
                new_msg.src = cpu1;
            else if (tmp_str == "dcache1")
                new_msg.src = cache1;
            else if (tmp_str == "icache1")
                new_msg.src = cache1;
            else if (tmp_str == "membus")
                new_msg.src = membus;
            else if (tmp_str == "mem")
                new_msg.src = mem;
            else
                throw std::invalid_argument("Unrecognized source component name " + tmp_str);
            
            tmp_str = line.substr(p4+1, p5-p4-1);

            if (tmp_str == "cpu0")
                new_msg.dest = cpu0;
            else if (tmp_str == "dcache0")
                new_msg.dest = cache0;
            else if (tmp_str == "icache0")
                new_msg.dest = cache0;
            else if (tmp_str == "cpu1")
                new_msg.dest = cpu1;
            else if (tmp_str == "dcache1")
                new_msg.dest = cache1;
            else if (tmp_str == "icache1")
                new_msg.dest = cache1;
            else if (tmp_str == "membus")
                new_msg.dest = membus;
            else if (tmp_str == "mem")
                new_msg.dest = mem;
            else
                throw std::invalid_argument("Unrecognized destination component name " + tmp_str);
            
            tmp_str = line.substr(p6+1, p7-p6-1);

            if (tmp_str == "readreq")
                new_msg.cmd = readreq ;
            else if (tmp_str == "readres")
                new_msg.cmd = readres;
            else if (tmp_str == "writereq")
                new_msg.cmd = writereq ;
            else if (tmp_str == "writeres")
                new_msg.cmd = writeres;
            else if (tmp_str == "StoreCondreq")
                new_msg.cmd = storeCondreq ;
            else if (tmp_str == "StoreCondres")
                new_msg.cmd = storeCondres;
            else if (tmp_str == "Upgradereq")
                new_msg.cmd = Upgradereq ;
            else if (tmp_str == "Upgraderes")
                new_msg.cmd = Upgraderes;
            else if (tmp_str == "readEXreq")
                new_msg.cmd = readExreq ;
            else if (tmp_str == "readExres")
                new_msg.cmd = readExres;
            else
                throw std::invalid_argument("Unrecognized command " + tmp_str);
            
            new_msg.addr = NDEF;
            


            trace.push_back(new_msg);
            
        }
        cout<<"finished"<<endl;
        trace_file.close();
    }
    else {
        cout << "Unable to open file" << endl;
        return 1;
    }
    
    cout << "Info: read " << trace.size() << " messages." << endl;
    
    vector<scenario_t> s_stack;
    stack<uint32_t> tri_stack;
    
    s_stack.push_back(scenario_t());
    tri_stack.push(0);
    
    vector<pair< vector<scenario_t>,uint32_t> >  bad_scenario_vec;
    
    // Matching message in the trace to scenairos.
    bool match = false;
    while (tri_stack.size() != 0) {
        match=false;
        uint32_t tri = tri_stack.top();
        tri_stack.pop();
        
        // If index tri reaches the end of trace, store the current scenario.
        if (tri == trace.size()) {
            //break if a scenario is found to match all messages.
            break;
        }
        s_stack=dscen(s_stack);
        vector<scenario_t> tmp_s_stack=s_stack;
        
        /**
         cout << endl << "***  processing tri = " << tri << endl;
         cout << "***  current scenario: " << endl;
         print_scenario(scenario);
         cout << endl;**/
        // Match the next message from trace against the current scenario.
        message_t msg(trace.at(tri));
        cout << "***  " << msg.toString() << endl << endl;
        
        vector<scenario_t> new_s_stack;
        for(uint32_t ct=0;ct<s_stack.size();ct++)
        {
            
            scenario_t scenario = s_stack.at(ct);
            
            
            
            // Match the enw_msg against the existing flow instances.
            for (uint32_t i = 0; i < scenario.size(); i++) {
                const flow_instance_t& f = scenario.at(i);
                config_t new_cfg = f.flow_inst->accept(msg, f.cfg);
                if (new_cfg != null_cfg) {
                    scenario_t new_scenario = scenario;
                    new_scenario.at(i).cfg = new_cfg;
                    match = true;
                    new_s_stack.push_back(new_scenario);
                    tri_stack.push(tri+1);
                    //cout << "Info: " << msg.toString() << "\t\t (" << f.flow_inst->get_flow_name() << ", " << f.inst_id << ")." << endl << flush;
                    //cout << "+++  new scenario (1) pushed to stack" << endl;
                    //print_scenario(new_scenario);
                }
                
            }
            if(s_stack.size()<15){
                // Create a new flow instance to match msg.
                for (uint32_t i = 0; i < flow_spec.size(); i++) {
                    lpn_t* f = flow_spec.at(i);
                    config_t new_cfg = f->accept(msg);
                    if (new_cfg != null_cfg) {
                        scenario_t new_scenario = scenario;
                        flow_instance_t new_f;
                        new_f.flow_inst = f;
                        ++flow_inst_cnt.at(i);
                        new_f.cfg = new_cfg;
                        new_scenario.push_back(new_f);
                        new_s_stack.push_back(new_scenario);
                        tri_stack.push(tri+1);
                        match = true;
                        
                        //cout << "Info: new instance (" << new_f.flow_inst->get_flow_name() << ", " << new_f.inst_id << ") is created" << endl << flush;
                        //cout << "Info: " << msg.toString() << "\t\t (" << new_f.flow_inst->get_flow_name() << ", " << new_f.inst_id << ")." << endl << flush;
                        
                        //cout << "+++  new scenario (0) pushed to stack" << endl;
                        //print_scenario(new_scenario);
                    }
                }
            }
            
        }
        
        if (match == false) {
            cout << "Info: " << trace.at(tri).toString() << " not matched, backtrack." << endl;
            pair< vector<scenario_t>,uint32_t> tmp_bad;
            tmp_bad.first=tmp_s_stack;
            tmp_bad.second=tri;
            bad_scenario_vec.push_back(tmp_bad);
            break;
        }
        else{
            s_stack=new_s_stack;
        }
        cout << "======================================" << endl;
    }
    if (s_stack.size() > 0) {
        cout << endl
        << "***  Success -  the scanario that matches all messages is" << endl;
        s_stack=dscen(s_stack);
        for(uint32_t ctt=0;ctt<s_stack.size();ctt++){
            scenario_t good_scen = s_stack.at(ctt);
            print_scenario(flow_spec, good_scen);
            cout << endl;
        }
    }
    
    else if (bad_scenario_vec.size()>0) {
        cout << endl
        << "***  Failed - generating the partial scanarios" << endl;
        pair<vector<scenario_t>,uint32_t> bad_scen= bad_scenario_vec.at(0);
        uint32_t msg_idx = bad_scen.second;
        message_t msg = trace.at(msg_idx);
        cout << "***  the following partial scenario failed to match message (" << msg_idx << ") " << msg.toString() << endl;
        
        for(uint32_t ctt=0;ctt<s_stack.size();ctt++){
            scenario_t tmp_print=bad_scen.first.at(ctt);
            print_scenario(flow_spec, tmp_print);
            cout << endl;}
    }
    return 0;
    
}



lpn_t* build_msi_flow_v1(void) {
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("... read");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1);
    msg1.src = mem;
    msg1.dest = cache1;
    msg1.cmd = readreq;
    msg1.addr = NDEF;
    lpn->insert_msg(msg1);
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1 << 2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = cache1;
    msg2.dest = membus;
    msg2.cmd = storeCondreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<6);
    msg3.src = cache0;
    msg3.dest = cpu0;
    msg3.cmd = storeCondreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    message_t msg4;
    msg4.pre_cfg = (1<<6);
    msg4.post_cfg = (1 << 9);
    msg4.src = membus;
    msg4.dest = cache0;
    msg4.cmd = storeCondreq;
    msg4.addr = NDEF;
    lpn->insert_msg(msg4);
    
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1 << 7);
    msg5.src = cache0;
    msg5.dest = cpu0;
    msg5.cmd = storeCondreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    message_t msg6;
    msg6.pre_cfg = (1<<7);
    msg6.post_cfg = (1 << 10);
    msg6.src = membus;
    msg6.dest = cache0;
    msg6.cmd = storeCondreq;
    msg6.addr = NDEF;
    lpn->insert_msg(msg6);
    
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 8);
    msg7.src = cache1;
    msg7.dest = cpu1;
    msg7.cmd = storeCondreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    message_t msg8;
    msg8.pre_cfg = (1<<8);
    msg8.post_cfg = (1 << 11);
    msg8.src = membus;
    msg8.dest = cache1;
    msg8.cmd = storeCondreq;
    msg8.addr = NDEF;
    lpn->insert_msg(msg8);
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1 << 12);
    msg9.src = cache1;
    msg9.dest = membus;
    msg9.cmd = storeCondreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
    message_t msg10;
    msg10.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg10.post_cfg = (1 << 13);
    msg10.src = NDEF;
    msg10.dest = NDEF;
    msg10.cmd = NDEF;
    msg10.addr = NDEF;
    lpn->insert_msg(msg10);
    
    message_t msg11;
    msg11.pre_cfg = (1<<13);
    msg11.post_cfg = (1 << 14);
    msg11.src = mem;
    msg11.dest = membus;
    msg11.cmd = readres;
    msg11.addr = NDEF;
    lpn->insert_msg(msg11);
    
    message_t msg12;
    msg12.pre_cfg = (1<<13);
    msg12.post_cfg = (1 << 15);
    msg12.src = cache0;
    msg12.dest = membus;
    msg12.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14)|(1<<15);
    msg13.post_cfg = (1 << 16);
    msg13.src = membus;
    msg13.dest = cache1;
    msg13.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    
    message_t msg14;
    msg14.pre_cfg = (1<<16)|(1<<1);
    msg14.post_cfg = (1 << 17);
    msg14.src = cache1;
    msg14.dest = cpu1;
    msg14.cmd = readres;
    msg14.addr = NDEF;
    lpn->insert_msg(msg14);
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
    
}


lpn_t* build_cpu0_read(void) {
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("cpu0 read");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1);
    msg1.src = cpu0;
    msg1.dest = cache0;
    msg1.cmd = readreq;
    msg1.addr = NDEF;
    lpn->insert_msg(msg1);
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1 << 2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = cache0;
    msg2.dest = membus;
    msg2.cmd = storeCondreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<6);
    msg3.src = cache1;
    msg3.dest = cpu1;
    msg3.cmd = storeCondreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    message_t msg4;
    msg4.pre_cfg = (1<<6);
    msg4.post_cfg = (1 << 9);
    msg4.src = membus;
    msg4.dest = cache1;
    msg4.cmd = storeCondreq;
    msg4.addr = NDEF;
    lpn->insert_msg(msg4);
    
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1 << 7);
    msg5.src = cache1;
    msg5.dest = cpu1;
    msg5.cmd = storeCondreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    message_t msg6;
    msg6.pre_cfg = (1<<7);
    msg6.post_cfg = (1 << 10);
    msg6.src = membus;
    msg6.dest = cache1;
    msg6.cmd = storeCondreq;
    msg6.addr = NDEF;
    lpn->insert_msg(msg6);
    
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 8);
    msg7.src = cache0;
    msg7.dest = cpu0;
    msg7.cmd = storeCondreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    message_t msg8;
    msg8.pre_cfg = (1<<8);
    msg8.post_cfg = (1 << 11);
    msg8.src = membus;
    msg8.dest = cache0;
    msg8.cmd = storeCondreq;
    msg8.addr = NDEF;
    lpn->insert_msg(msg8);
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1 << 12);
    msg9.src = cache0;
    msg9.dest = membus;
    msg9.cmd = storeCondreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
    message_t msg10;
    msg10.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg10.post_cfg = (1 << 13);
    msg10.src = NDEF;
    msg10.dest = NDEF;
    msg10.cmd = NDEF;
    msg10.addr = NDEF;
    lpn->insert_msg(msg10);
    
    message_t msg11;
    msg11.pre_cfg = (1<<13);
    msg11.post_cfg = (1 << 14);
    msg11.src = mem;
    msg11.dest = membus;
    msg11.cmd = readres;
    msg11.addr = NDEF;
    lpn->insert_msg(msg11);
    
    message_t msg12;
    msg12.pre_cfg = (1<<13);
    msg12.post_cfg = (1 << 15);
    msg12.src = cache1;
    msg12.dest = membus;
    msg12.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14)|(1<<15);
    msg13.post_cfg = (1 << 16);
    msg13.src = membus;
    msg13.dest = cache0;
    msg13.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    
    message_t msg14;
    msg14.pre_cfg = (1<<16)|(1<<1);
    msg14.post_cfg = (1 << 17);
    msg14.src = cache0;
    msg14.dest = cpu0;
    msg14.cmd = readres;
    msg14.addr = NDEF;
    lpn->insert_msg(msg14);
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}

lpn_t* build_cpu1_read(void) {
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu1 read*******");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1);
    msg1.src = cpu1;
    msg1.dest = cache1;
    msg1.cmd = readreq;
    msg1.addr = NDEF;
    lpn->insert_msg(msg1);
    cout<<"msg1"<<endl;
    cout<<"src "<<msg1.src<<endl;
    cout<<"dest "<<msg1.dest<<endl;
    cout<<"pre "<<msg1.pre_cfg<<endl;
    cout<<"cmd "<<msg1.cmd<<endl;
    cout<<endl;
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1<<2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = cache1;
    msg2.dest = membus;
    msg2.cmd = storeCondreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    cout<<"msg2"<<endl;
    cout<<"src "<<msg2.src<<endl;
    cout<<"dest "<<msg2.dest<<endl;
    cout<<"pre "<<msg2.pre_cfg<<endl;
    cout<<"cmd "<<msg2.cmd<<endl;
    cout<<endl;
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<6);
    msg3.src = membus;
    msg3.dest = cache0;
    msg3.cmd = storeCondreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    cout<<"msg3"<<endl;
    cout<<"src "<<msg3.src<<endl;
    cout<<"dest "<<msg3.dest<<endl;
    cout<<"pre "<<msg3.pre_cfg<<endl;
    cout<<"cmd "<<msg3.cmd<<endl;
    cout<<endl;

    
    message_t msg4;
    msg4.pre_cfg = (1<<6);
    msg4.post_cfg = (1<<9);
    msg4.src = cache0;
    msg4.dest = cpu0;
    msg4.cmd = storeCondreq;
    msg4.addr = NDEF;
    lpn->insert_msg(msg4);
    
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1<<7);
    msg5.src = membus;
    msg5.dest = cache0;
    msg5.cmd = storeCondreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    message_t msg6;
    msg6.pre_cfg = (1<<7);
    msg6.post_cfg = (1<<10);
    msg6.src = cache0;
    msg6.dest = cpu0;
    msg6.cmd = storeCondreq;
    msg6.addr = NDEF;
    lpn->insert_msg(msg6);
    
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 8);
    msg7.src = membus;
    msg7.dest = cache1;
    msg7.cmd = storeCondreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    

    message_t msg8;
    msg8.pre_cfg = (1<<8);
    msg8.post_cfg = (1<<11);
    msg8.src = cache1;
    msg8.dest = cpu1;
    msg8.cmd = storeCondreq;
    msg8.addr = NDEF;
    lpn->insert_msg(msg8);
    
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1<<12);
    msg9.src = membus;
    msg9.dest = mem;
    msg9.cmd = storeCondreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
    /**
    message_t msg10;
    msg10.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg10.post_cfg = (1 << 13);
    msg10.src = NDEF;
    msg10.dest = NDEF;
    msg10.cmd = NDEF;
    msg10.addr = NDEF;
    lpn->insert_msg(msg10);
    **/
    
    message_t msg11;
    msg11.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg11.post_cfg = (1 << 14);
    msg11.src = mem;
    msg11.dest = membus;
    msg11.cmd = readres;
    msg11.addr = NDEF;
    lpn->insert_msg(msg11);
    
    message_t msg12;
    msg12.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg12.post_cfg = (1 << 14);
    msg12.src = cache0;
    msg12.dest = membus;
    msg12.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14);
    msg13.post_cfg = (1 << 15);
    msg13.src = membus;
    msg13.dest = cache1;
    msg13.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    
    message_t msg14;
    msg14.pre_cfg = (1<<15);
    msg14.post_cfg = (1 << 16);
    msg14.src = cache1;
    msg14.dest = cpu1;
    msg14.cmd = readres;
    msg14.addr = NDEF;
    lpn->insert_msg(msg14);
                
                     message_t msg15;
                     msg14.pre_cfg = (1<<1);
                     msg14.post_cfg = (1 << 17);
                     msg14.src = cache1;
                     msg14.dest = cpu1;
                     msg14.cmd = readres;
                     msg14.addr = NDEF;
                     lpn->insert_msg(msg14);
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}

lpn_t* build_us_mem_rd_flow_v1(void)
{
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("cpu1 read");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1);
    msg1.src = cpu1;
    msg1.dest = cache1;
    msg1.cmd = readreq;
    msg1.addr = NDEF;
    lpn->insert_msg(msg1);
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1 << 2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = cache1;
    msg2.dest = membus;
    msg2.cmd = storeCondreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<6);
    msg3.src = cache0;
    msg3.dest = cpu0;
    msg3.cmd = storeCondreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    message_t msg4;
    msg4.pre_cfg = (1<<6);
    msg4.post_cfg = (1 << 9);
    msg4.src = membus;
    msg4.dest = cache0;
    msg4.cmd = storeCondreq;
    msg4.addr = NDEF;
    lpn->insert_msg(msg4);
    
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1 << 7);
    msg5.src = cache0;
    msg5.dest = cpu0;
    msg5.cmd = storeCondreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    message_t msg6;
    msg6.pre_cfg = (1<<7);
    msg6.post_cfg = (1 << 10);
    msg6.src = membus;
    msg6.dest = cache0;
    msg6.cmd = storeCondreq;
    msg6.addr = NDEF;
    lpn->insert_msg(msg6);
    
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 8);
    msg7.src = cache1;
    msg7.dest = cpu1;
    msg7.cmd = storeCondreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    message_t msg8;
    msg8.pre_cfg = (1<<8);
    msg8.post_cfg = (1 << 11);
    msg8.src = membus;
    msg8.dest = cache1;
    msg8.cmd = storeCondreq;
    msg8.addr = NDEF;
    lpn->insert_msg(msg8);
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1 << 12);
    msg9.src = cache1;
    msg9.dest = membus;
    msg9.cmd = storeCondreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
    message_t msg10;
    msg10.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg10.post_cfg = (1 << 13);
    msg10.src = NDEF;
    msg10.dest = NDEF;
    msg10.cmd = NDEF;
    msg10.addr = NDEF;
    lpn->insert_msg(msg10);
    
    message_t msg11;
    msg11.pre_cfg = (1<<13);
    msg11.post_cfg = (1 << 14);
    msg11.src = mem;
    msg11.dest = membus;
    msg11.cmd = readres;
    msg11.addr = NDEF;
    lpn->insert_msg(msg11);
    
    message_t msg12;
    msg12.pre_cfg = (1<<13);
    msg12.post_cfg = (1 << 15);
    msg12.src = cache0;
    msg12.dest = membus;
    msg12.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14)|(1<<15);
    msg13.post_cfg = (1 << 16);
    msg13.src = membus;
    msg13.dest = cache1;
    msg13.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    
    message_t msg14;
    msg14.pre_cfg = (1<<16)|(1<<1);
    msg14.post_cfg = (1 << 17);
    msg14.src = cache1;
    msg14.dest = cpu1;
    msg14.cmd = readres;
    msg14.addr = NDEF;
    lpn->insert_msg(msg14);
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}


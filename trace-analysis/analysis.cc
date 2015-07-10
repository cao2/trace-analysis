
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include "lpn.h"

// build a lpn model for each flow.
lpn_t* build_msi_flow_v1(void);
lpn_t* build_ds_mmio_rd_flow_v1(void);
lpn_t* build_us_mem_wr_flow_v1(void);
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
        return (flow_inst == other.flow_inst &&
                cfg == other.cfg);
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

int main(int argc, char *argv[]) {
    
    // Build flow specification
    vector<lpn_t*> flow_spec;
    lpn_t* msi_flow = build_msi_flow_v1();
    lpn_t* ds_mmio_rd = build_ds_mmio_rd_flow_v1();
    lpn_t* us_mem_wr = build_us_mem_wr_flow_v1();
    lpn_t* us_mem_rd = build_us_mem_rd_flow_v1();
    flow_spec.push_back(msi_flow);
    msi_flow->set_index(0);
    flow_spec.push_back(ds_mmio_rd);
    ds_mmio_rd->set_index(1);
    flow_spec.push_back(us_mem_wr);
    us_mem_wr->set_index(2);
    flow_spec.push_back(us_mem_rd);
    us_mem_rd->set_index(3);
    
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
            uint32_t p1, p2,  p3;
            p1=p2=p3=0;
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
            
            string tmp_str = line.substr(0, p1);
            if (tmp_str == "CPU0")
                new_msg.src = CPU;
            else if (tmp_str == "SSA")
                new_msg.src = SSA;
            else if (tmp_str == "GFX")
                new_msg.src = GFX;
            else if (tmp_str == "DUNIT")
                new_msg.src = DUNIT;
            else
                throw std::invalid_argument("Unrecognized source component name " + tmp_str);
            
            tmp_str = line.substr(p1+1, p2-p1-1);
            if (tmp_str == "CPU0")
                new_msg.dest = CPU;
            else if (tmp_str == "SSA")
                new_msg.dest = SSA;
            else if (tmp_str == "GFX")
                new_msg.dest = GFX;
            else if (tmp_str == "DUNIT")
                new_msg.dest = DUNIT;
            else
                throw std::invalid_argument("Unrecognized destination component name " + tmp_str);

            
            tmp_str = line.substr(p2+1, p3-p2-1);
            if (tmp_str == "RD")
                new_msg.cmd = RD;
            else if (tmp_str == "WR")
                new_msg.cmd = WR;
            else if (tmp_str == "DataValid")
                new_msg.cmd = DataValid;
            else if (tmp_str == "ReplyHit")
                new_msg.cmd = ReplyHit;
            else if (tmp_str == "ReplyMiss")
                new_msg.cmd = ReplyMiss;
            else if (tmp_str == "MSI")
                new_msg.cmd = MSI;
            else
                throw std::invalid_argument("Unrecognized command " + tmp_str);
            
            tmp_str = line.substr(p3+1, line.size()-p3-1);
            if (tmp_str == "-")
                new_msg.addr = NDEF;
            else if (tmp_str == "INTR")
                new_msg.addr = INTR;
            else if (tmp_str == "DMEM")
                new_msg.addr = DMEM;
            else if (tmp_str == "MMIO")
                new_msg.addr = MMIO;
            else
                throw std::invalid_argument("Unrecognized memory address range " + tmp_str);
            
            trace.push_back(new_msg);
            
        }
        trace_file.close();
    }
    else {
        cout << "Unable to open file" << endl;
        return 1;
    }
    
    cout << "Info: read " << trace.size() << " messages." << endl;
    
    stack<scenario_t> s_stack;
    stack<uint32_t> tri_stack;
    
    s_stack.push(scenario_t());
    tri_stack.push(0);
    
    vector<scenario_t> good_scenario_vec;
    vector<pair<scenario_t, uint32_t> > bad_scenario_vec;

    // Matching message in the trace to scenairos.
    while (tri_stack.size() != 0) {
        scenario_t scenario = s_stack.top();
        uint32_t tri = tri_stack.top();
        
        s_stack.pop();
        tri_stack.pop();
        
        // If index tri reaches the end of trace, store the current scenario.
        if (tri == trace.size()) {
            good_scenario_vec.push_back(scenario);
            //break if a scenario is found to match all messages.
            break;
        }
        
        cout << endl << "***  processing tri = " << tri << endl;
        cout << "***  current scenario: " << endl;
        print_scenario(scenario);
        cout << endl;
        
        // Match the next message from trace against the current scenario.
        message_t msg(trace.at(tri));
        
        cout << "***  " << msg.toString() << endl << endl;
        
        bool match = false;
        // Match the enw_msg against the existing flow instances.
        for (uint32_t i = 0; i < scenario.size(); i++) {
            const flow_instance_t& f = scenario.at(i);
            config_t new_cfg = f.flow_inst->accept(msg, f.cfg);
            if (new_cfg != null_cfg) {
                scenario_t new_scenario = scenario;
                new_scenario.at(i).cfg = new_cfg;
                match = true;
                s_stack.push(new_scenario);
                tri_stack.push(tri+1);
                //cout << "Info: " << msg.toString() << "\t\t (" << f.flow_inst->get_flow_name() << ", " << f.inst_id << ")." << endl << flush;
                cout << "+++  new scenario (1) pushed to stack" << endl;
                print_scenario(new_scenario);
            }

        }
        
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
                s_stack.push(new_scenario);
                tri_stack.push(tri+1);
                match = true;
                
                //cout << "Info: new instance (" << new_f.flow_inst->get_flow_name() << ", " << new_f.inst_id << ") is created" << endl << flush;
                //cout << "Info: " << msg.toString() << "\t\t (" << new_f.flow_inst->get_flow_name() << ", " << new_f.inst_id << ")." << endl << flush;
                
                cout << "+++  new scenario (0) pushed to stack" << endl;
                print_scenario(new_scenario);
            }
        }
        
        if (match == false) {
            cout << "Info: " << msg.toString() << " not matched, backtrack." << endl;
            bad_scenario_vec.push_back(make_pair(scenario, tri));
        }
        
        cout << "======================================" << endl;
    }
    
    
    if (good_scenario_vec.size() > 0) {
        cout << endl
            << "***  Success -  the scanario that matches all messages is" << endl;
        scenario_t good_scen = good_scenario_vec.front();
        print_scenario(flow_spec, good_scen);
    }
    
    else if (bad_scenario_vec.size() > 0) {
        cout << endl
        << "***  Failed - generating the partial scanarios" << endl;
        for (uint32_t i = 0; i < bad_scenario_vec.size(); i++) {
            pair<scenario_t, uint32_t> bad_scen = bad_scenario_vec.at(i);
            uint32_t msg_idx = bad_scen.second;
            message_t msg = trace.at(msg_idx);
            cout << "***  the following partial scenario failed to match message (" << msg_idx << ") " << msg.toString() << endl;
            print_scenario(flow_spec, bad_scen.first);
            cout << endl;
        }
    }
    return 0;
}



lpn_t* build_msi_flow_v1(void) {
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("Upstream MSI");
    
    message_t msg;
    msg.pre_cfg = (1<<0);
    msg.post_cfg = (1<<1);
    msg.src = GFX;
    msg.dest = SSA;
    msg.cmd = WR;
    msg.addr = INTR;
    lpn->insert_msg(msg);
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1<<2);
    msg2.src = SSA;
    msg2.dest = CPU;
    msg2.cmd = MSI;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<3);
    msg3.src = CPU;
    msg3.dest = SSA;
    msg3.cmd = ReplyMiss;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    lpn->set_init_cfg(1<<0);
    return lpn;
}


lpn_t* build_ds_mmio_rd_flow_v1(void) {
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("Downstream MMIO Read");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1) | (1 << 2);
    msg1.src = CPU;
    msg1.dest = SSA;
    msg1.cmd = RD;
    msg1.addr = MMIO;
    lpn->insert_msg(msg1);
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1 << 3);
    msg2.src = SSA;
    msg2.dest = CPU;
    msg2.cmd = RD;
    msg2.addr = MMIO;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<3);
    msg3.post_cfg = (1 << 5);
    msg3.src = CPU;
    msg3.dest = SSA;
    msg3.cmd = ReplyMiss;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    message_t msg4;
    msg4.pre_cfg = (1<<2);
    msg4.post_cfg = (1 << 4);
    msg4.src = SSA;
    msg4.dest = GFX;
    msg4.cmd = RD;
    msg4.addr = MMIO;
    lpn->insert_msg(msg4);
    
    message_t msg5;
    msg5.pre_cfg = (1<<4);
    msg5.post_cfg = (1 << 6);
    msg5.src = GFX;
    msg5.dest = SSA;
    msg5.cmd = DataValid;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    message_t msg6;
    msg6.pre_cfg = (1<<5) | (1<<6);
    msg6.post_cfg = (1 << 7);
    msg6.src = SSA;
    msg6.dest = CPU;
    msg6.cmd = DataValid;
    msg6.addr = NDEF;
    lpn->insert_msg(msg6);
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}


lpn_t* build_us_mem_wr_flow_v1(void)
{
    lpn_t *lpn = new lpn_t;
    
    lpn->set_flow_name("Upstream Memory Write");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1) | (1 << 2);
    msg1.src = GFX;
    msg1.dest = SSA;
    msg1.cmd = WR;
    msg1.addr = DMEM;
    lpn->insert_msg(msg1);
    
    message_t msg1_1;
    msg1_1.pre_cfg = (1<<2);
    msg1_1.post_cfg = (1 << 6);
    msg1_1.src = GFX;
    msg1_1.dest = SSA;
    msg1_1.cmd = DataValid;
    msg1_1.addr = NDEF;
    lpn->insert_msg(msg1_1);
    
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1 << 3);
    msg2.src = SSA;
    msg2.dest = CPU;
    msg2.cmd = WR;
    msg2.addr = DMEM;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<3);
    msg3.post_cfg = (1 << 4);
    msg3.src = CPU;
    msg3.dest = SSA;
    msg3.cmd = ReplyHit;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    message_t msg4;
    msg4.pre_cfg = (1<<3);
    msg4.post_cfg = (1<<5);
    msg4.src = CPU;
    msg4.dest = SSA;
    msg4.cmd = ReplyMiss;
    msg4.addr = NDEF;
    lpn->insert_msg(msg4);
    
    message_t msg5;
    msg5.pre_cfg = (1<<4);
    msg5.post_cfg = (1<<5);
    msg5.src = CPU;
    msg5.dest = SSA;
    msg5.cmd = DataValid;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    message_t msg6;
    msg6.pre_cfg = (1<<5) | (1<<6);
    msg6.post_cfg = (1<<7);
    msg6.src = SSA;
    msg6.dest = DUNIT;
    msg6.cmd = DataValid;
    msg6.addr = NDEF;
    lpn->insert_msg(msg6);
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}


lpn_t* build_us_mem_rd_flow_v1(void)
{
    lpn_t *lpn = new lpn_t;
    
    lpn->set_flow_name("Upstream Memory Read");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1) | (1 << 6);
    msg1.src = GFX;
    msg1.dest = SSA;
    msg1.cmd = RD;
    msg1.addr = DMEM;
    lpn->insert_msg(msg1);
    
    message_t msg1_1;
    msg1_1.pre_cfg = (1<<6);
    msg1_1.post_cfg = (1<<7);
    msg1_1.src = SSA;
    msg1_1.dest = DUNIT;
    msg1_1.cmd = RD;
    msg1_1.addr = DMEM;
    lpn->insert_msg(msg1_1);
    
    message_t msg2;
    msg2.pre_cfg = (1<<1);
    msg2.post_cfg = (1 << 2);
    msg2.src = SSA;
    msg2.dest = CPU;
    msg2.cmd = RD;
    msg2.addr = DMEM;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<3);
    msg3.src = CPU;
    msg3.dest = SSA;
    msg3.cmd = ReplyHit;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    message_t msg4;
    msg4.pre_cfg = (1<<2);
    msg4.post_cfg = (1<<4);
    msg4.src = CPU;
    msg4.dest = SSA;
    msg4.cmd = ReplyMiss;
    msg4.addr = NDEF;
    lpn->insert_msg(msg4);
    
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1<<4);
    msg5.src = CPU;
    msg5.dest = SSA;
    msg5.cmd = DataValid;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    message_t msg6;
    msg6.pre_cfg = (1<<7);
    msg6.post_cfg = (1<<8);
    msg6.src = DUNIT;
    msg6.dest = SSA;
    msg6.cmd = DataValid;
    msg6.addr = NDEF;
    lpn->insert_msg(msg6);
    
    message_t msg7;
    msg7.pre_cfg = (1<<4) | (1<<8);
    msg7.post_cfg = (1<<5);
    msg7.src = SSA;
    msg7.dest = GFX;
    msg7.cmd = DataValid;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    message_t msg8;
    msg8.pre_cfg = (1<<5);
    msg8.post_cfg = (1<<10);
    msg8.src = SSA;
    msg8.dest = DUNIT;
    msg8.cmd = DataValid;
    msg8.addr = NDEF;
    lpn->insert_msg(msg8);
    
    message_t msg9;
    msg9.pre_cfg = (1<<4) | (1<<8);
    msg9.post_cfg = (1<<9);
    msg9.src = SSA;
    msg9.dest = DUNIT;
    msg9.cmd = DataValid;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
    message_t msg10;
    msg10.pre_cfg = (1<<9);
    msg10.post_cfg = (1<<10);
    msg10.src = SSA;
    msg10.dest = GFX;
    msg10.cmd = DataValid;
    msg10.addr = NDEF;
    lpn->insert_msg(msg10);
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}



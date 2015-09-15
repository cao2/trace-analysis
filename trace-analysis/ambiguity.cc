#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include "lpn.h"
#include <set>
#include <algorithm>    // std::sort
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
// build a lpn model for each flow.
lpn_t* build_msi_flow_v1(void);
lpn_t* build_cpu0_read(void);
lpn_t* build_cpu1_read(void);
lpn_t* build_us_mem_rd_flow_v1(void);
lpn_t* build_cpu0_dread(void);
lpn_t* build_cpu1_dread(void);
lpn_t* build_cpu0_storefail(void);
lpn_t* build_cpu1_storefail(void);
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
        
        return (flow_inst->get_flow_name() == other.flow_inst->get_flow_name() &&cfg == other.cfg);
    }
    
    flow_instance_t& operator=(const flow_instance_t& other) {
        flow_inst = other.flow_inst;
        cfg = other.cfg;
        return *this;
    }
    
};
struct scenario_t{
    uint32_t read1;
    uint32_t read0;
    uint32_t write0;
    uint32_t write1;
    uint32_t read0d;
    uint32_t read1d;
    uint32_t fail0;
    uint32_t fail1;
    uint32_t total_inactive;
    vector<flow_instance_t> active_t;
    
    scenario_t(){
        read0=0;
        read1=0;
        write0=0;
        write1=0;
        read0d=0;
        read1d=0;
        fail0=0;
        fail1=0;
        total_inactive=0;
    }
    
};
//typedef vector<flow_instance_t> scenario_t;

void print_scenario(const scenario_t& sce)
{
    vector<flow_instance_t> scen=sce.active_t;
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


void print_scenario(const vector<lpn_t*> flow_spec, const scenario_t& sce)
{
    vector<flow_instance_t> scen=sce.active_t;
    
    vector<uint32_t> flow_inst_cnt;
    flow_inst_cnt.push_back(sce.write0);
    flow_inst_cnt.push_back(sce.read0);
    flow_inst_cnt.push_back(sce.write1);
    flow_inst_cnt.push_back(sce.read1);
    flow_inst_cnt.push_back(sce.read0d);
    flow_inst_cnt.push_back(sce.read1d);
    flow_inst_cnt.push_back(sce.fail0);
    flow_inst_cnt.push_back(sce.fail1);
    
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

std::hash<std::string> str_hash;

bool equalscen(const scenario_t &x, const scenario_t &y){
    if(x.read0!=y.read0||x.read1!=y.read1||x.write0!=y.write0||x.write1!=y.write1)
        return false;
    if(x.active_t.size()!=y.active_t.size())
        return false;
    for(uint32_t i=0;i<x.active_t.size();i++){
        if(x.active_t.at(i).flow_inst->get_flow_name()!=y.active_t.at(i).flow_inst->get_flow_name())
            return false;
        
        if(x.active_t.at(i).cfg!=y.active_t.at(i).cfg)
            return false;
        
    }
    
    
    return true;
}
vector<scenario_t> dscen(const vector<scenario_t> &vec){
    //sort( vec.begin(), vec.end() );
    // vec.erase(vec.begi
    vector<scenario_t> rst;
    rst.push_back(vec.at(0));
    for(uint32_t i=1;i<vec.size();i++){
        bool flag=true;
        for(uint32_t j=0; j< rst.size(); j++){
            if(equalscen(vec.at(i),rst.at(j))==false);
            else{
                flag=false;
                break;}
        }
        if (flag==true)
        {rst.push_back(vec.at(i));
        }
    }
    return rst;
}

string cfg_str_c(const uint32_t& xcfg){
    uint32_t cfg=xcfg;
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
    return cfg_str;
}
int main(int argc, char *argv[]) {
    struct rusage r_usage;
    // Build flow specification
    vector<lpn_t*> flow_spec;
    vector<uint32_t> noneed_monitors;
    
    lpn_t* msi_flow = build_msi_flow_v1();
    lpn_t* cpu0_read = build_cpu0_read();
    lpn_t* us_mem_wr=build_us_mem_rd_flow_v1();
    lpn_t* cpu1_read=build_cpu1_read();
    lpn_t* cpu0_dread=build_cpu0_dread();
    lpn_t* cpu1_dread=build_cpu1_dread();
    lpn_t* cpu0_fail=build_cpu0_storefail();
    lpn_t* cpu1_fail=build_cpu1_storefail();
    flow_spec.push_back(msi_flow);//write0
    msi_flow->set_index(0);
    flow_spec.push_back(cpu0_read);
    cpu0_read->set_index(1);
    flow_spec.push_back(us_mem_wr);
    us_mem_wr->set_index(2);
    flow_spec.push_back(cpu1_read);//write1
    cpu1_read->set_index(3);
    flow_spec.push_back(cpu0_dread);
    cpu0_dread->set_index(4);
    flow_spec.push_back(cpu1_dread);
    cpu1_dread->set_index(5);
    flow_spec.push_back(cpu0_fail);
    cpu0_fail->set_index(6);
    flow_spec.push_back(cpu1_fail);
    cpu1_fail->set_index(7);
    
    vector<uint32_t> flow_inst_cnt;
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    flow_inst_cnt.push_back(0);
    
    ofstream errorfile;
    errorfile.open ("erromsg.txt",ios::trunc);
    
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
            
            string tmp_str = line.substr(p3+1, p4-p3-1);
            
            if (tmp_str == "cpu0")
                new_msg.src = cpu0;
            else if (tmp_str == "dcache0")
                new_msg.src = dcache0;
            else if (tmp_str == "icache0")
                new_msg.src = icache0;
            else if (tmp_str == "cpu1")
                new_msg.src = cpu1;
            else if (tmp_str == "dcache1")
                new_msg.src = dcache1;
            else if (tmp_str == "icache1")
                new_msg.src = icache1;
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
                new_msg.dest = dcache0;
            else if (tmp_str == "icache0")
                new_msg.dest = icache0;
            else if (tmp_str == "cpu1")
                new_msg.dest = cpu1;
            else if (tmp_str == "dcache1")
                new_msg.dest = dcache1;
            else if (tmp_str == "icache1")
                new_msg.dest = icache1;
            else if (tmp_str == "membus")
                new_msg.dest = membus;
            else if (tmp_str == "mem")
                new_msg.dest = mem;
            else
                throw std::invalid_argument("Unrecognized destination component name " + tmp_str);
            
            tmp_str = line.substr(p6+1, p7-p6-1);
            
            if (tmp_str == "read")
                new_msg.cmd = read ;
            else if (tmp_str == "write")
                new_msg.cmd = write ;
            else if (tmp_str == "StoreCondFailreq")
                new_msg.cmd = StoreCondFailreq;
            else if (tmp_str == "iFunc")
                new_msg.cmd = iFunc ;
            else if (tmp_str == "uFunc")
                new_msg.cmd = uFunc;
            
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
    int maxsize=0;
    s_stack.push_back(scenario_t());
    tri_stack.push(0);
    
    vector<pair< vector<scenario_t>,uint32_t> >  bad_scenario_vec;
    
    // Matching message in the trace to scenairos.
    bool match = false;
    int erromsg=0;
    while (tri_stack.size() != 0) {
        //for(uint32_t niuniu=0;niuniu<100;niuniu++){
        match=false;
        uint32_t tri = tri_stack.top();
        tri_stack.pop();
        
        // If index tri reaches the end of trace, store the current scenario.
        if (tri == trace.size()) {
            //break if a scenario is found to match all messages.
            break;
        }
        if (tri%10==0){
            
            
            cout<<"************************"<<endl;
            cout<<"dscen called, orig size"<<s_stack.size()<<endl;
            s_stack=dscen(s_stack);
            cout<<"************************"<<endl;
            
            
            cout<<"NEW size"<<s_stack.size()<<endl;
        }
        
        vector<scenario_t> tmp_s_stack=s_stack;
        message_t msg(trace.at(tri));
        cout << tri<<"***  " << msg.toString() <<"  "<< s_stack.size() <<endl << endl;
        
        
        vector<message_t> msg_vec;
        
        if (msg.cmd==read){
            //include readreq, readres, writereq, writeres
            message_t n_msg=msg;
            n_msg.cmd=readreq;
            message_t m_msg=msg;
            m_msg.cmd=readres;
            message_t x_msg=msg;
            x_msg.cmd=writereq;
            message_t y_msg=msg;
            y_msg.cmd=writeres;
            
            msg_vec.push_back(n_msg);
            msg_vec.push_back(m_msg);
            msg_vec.push_back(x_msg);
            msg_vec.push_back(y_msg);
        }
        else if(msg.cmd==StoreCondFailreq){
            msg_vec.push_back(msg);
        }
        else if(msg.cmd==iFunc){
            message_t n_msg=msg;
            n_msg.cmd=readExreq;
            message_t m_msg=msg;
            m_msg.cmd=Upgradereq;
            
            message_t y_msg=msg;
            y_msg.cmd=Upgraderes;
            message_t z_msg=msg;
            z_msg.cmd=readExres;
            
            
            msg_vec.push_back(n_msg);
            msg_vec.push_back(m_msg);
            msg_vec.push_back(y_msg);
            msg_vec.push_back(z_msg);
            

        }
        else if(msg.cmd==uFunc){
            message_t x_msg=msg;
            x_msg.cmd=storeCondreq;
            message_t g_msg=msg;
            g_msg.cmd=loadLockedreq;
            message_t f_msg=msg;
            f_msg.cmd=readres;
            msg_vec.push_back(x_msg);
            msg_vec.push_back(g_msg);
            msg_vec.push_back(f_msg);

        }
        else{
            cout<<"no such func"<<endl;
        }
        
        
        vector<scenario_t> new_s_stack;
        
        int flow_spec_flag[8];
        int yuting;
    for(yuting=0;yuting<msg_vec.size();yuting++){
        msg=msg_vec.at(yuting);
        bool tmpflag=false;
         cout << tri<<"***  " << msg.toString() <<"  "<< s_stack.size() <<endl << endl;
        //find out if new msg can create a new flow_inst
        for (uint32_t i = 0; i < flow_spec.size(); i++) {
            lpn_t* f = flow_spec.at(i);
            config_t new_cfg = f->accept(msg);
            if (new_cfg != null_cfg)
                flow_spec_flag[i]=new_cfg;
            else
                flow_spec_flag[i]=99;
        }
        // Match the next message from trace against the current scenario.
        for(uint32_t ct=0;ct<s_stack.size();ct++)
        {
            
            scenario_t scenario = s_stack.at(ct);
            // Match the enw_msg against the existing flow instances.
            for (uint32_t i = 0; i < scenario.active_t.size(); i++) {
                const flow_instance_t& f = scenario.active_t.at(i);
                config_t new_cfg = f.flow_inst->accept(msg, f.cfg);
                if (new_cfg != null_cfg) {
                    uint32_t flow_index = f.flow_inst->get_index();
                    scenario_t new_scenario = scenario;
                    
                    string cfg_str=cfg_str_c(new_cfg);
                    if(cfg_str=="16" ||cfg_str=="17" || cfg_str=="31")
                    {
                        if(flow_index==0)
                            new_scenario.write0++;
                        else if(flow_index==1)
                            new_scenario.read0++;
                        else if(flow_index==2)
                            new_scenario.write1++;
                        else if(flow_index==3)
                            new_scenario.read1++;
                        else if(flow_index==4)
                            new_scenario.read0d++;
                        else if(flow_index==5)
                            new_scenario.read1d++;
                        else if(flow_index==6)
                            new_scenario.fail0++;
                        else if(flow_index==7)
                            new_scenario.fail1++;
                        new_scenario.active_t.erase(new_scenario.active_t.begin()+i);
                    }
                    
                    else{
                        new_scenario.active_t.at(i).cfg = new_cfg;
                    }
                    match = true;
                    tmpflag=true;
                    new_s_stack.push_back(new_scenario);
                    tri_stack.push(tri+1);
                    //cout << "Info: " << msg.toString() << "\t\t (" << f.flow_inst->get_flow_name() << ", " << f.inst_id << ")." << endl << flush;
                    //cout << "+++  new scenario (1) pushed to stack" << endl;
                    //print_scenario(new_scenario);
                }
                
            }
            // Create a new flow instance to match msg.
            /**82
            ****cpu0 read*******: 	169
            ****cpu1 write*******: 	83
            ****cpu1 read*******: 	190
            ****cpu0 read to dcache*******: 	829
            ****cpu1 read to dcache*******: 	803
            ****cpu0 storeCondfail******: 	0
**/
            for(uint32_t i=0;i<flow_spec.size();i++){
                if(flow_spec_flag[i]!=99){
                    scenario_t new_scenario = scenario;
                    flow_instance_t new_f;
                    new_f.flow_inst = flow_spec.at(i);
                    ++flow_inst_cnt.at(i);
                    new_f.cfg = flow_spec_flag[i];
                    new_scenario.active_t.push_back(new_f);
                    new_s_stack.push_back(new_scenario);
                    tri_stack.push(tri+1);
                    match = true;
                    tmpflag=true;
                }
            }
            
        }
        if( tmpflag==false)
            erromsg++;
    }
        if (match == false) {
            tri_stack.push(tri+1);
            cout << "Info: " << trace.at(tri).toString() << " not matched, backtrack." << endl;
            pair< vector<scenario_t>,uint32_t> tmp_bad;
            tmp_bad.first=tmp_s_stack;
            tmp_bad.second=tri;
            bad_scenario_vec.push_back(tmp_bad);
            break;
            errorfile<<trace.at(tri).toString()<<"line #:"<<tri<<"\n";
            
        }
        else{
            s_stack=new_s_stack;
            if(s_stack.size()>maxsize)
                maxsize=s_stack.size();
        }
        cout << "======================================" << endl;
    }
    errorfile.close();
    if (s_stack.size() > 0) {
        cout << endl
        << "***  Success -  the scenario that matches all messages is" << endl;
        s_stack=dscen(s_stack);
        for(uint32_t ctt=0;ctt<s_stack.size();ctt++){
            scenario_t good_scen = s_stack.at(ctt);
            print_scenario(flow_spec, good_scen);
            cout << endl;
        }
        cout<<"max number of scenarios is "<<maxsize<<endl;
        cout<<"total number of messge not matches: "<<erromsg<<endl;
    }
    
    else if (bad_scenario_vec.size()>0) {
        cout << endl
        << "***  Failed - generating the partial scenarios" << endl;
        pair<vector<scenario_t>,uint32_t> bad_scen= bad_scenario_vec.at(0);
        uint32_t msg_idx = bad_scen.second;
        message_t msg = trace.at(msg_idx);
        cout << "***  the following partial scenario failed to match message (" << msg_idx << ") " << msg.toString() << endl;
        
        for(uint32_t ctt=0;ctt<s_stack.size();ctt++){
            scenario_t tmp_print=bad_scen.first.at(ctt);
            print_scenario(flow_spec, tmp_print);
            cout << endl;}
    }
    getrusage(RUSAGE_SELF,&r_usage);
    printf("************************Memory usage = %ld\n",r_usage.ru_maxrss);

    return 0;
    
}


lpn_t* build_msi_flow_v1(void) {
    
    
    
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu0 write*******");
    /**
     message_t msg1;
     msg1.pre_cfg = (1<<0);
     msg1.post_cfg = (1 << 1);
     msg1.src = cpu0;
     msg1.dest = icache0;
     msg1.cmd = writereq;
     msg1.addr = NDEF;
     lpn->insert_msg(msg1);
     **/
    
    message_t msg2;
    msg2.pre_cfg = (1<<0);
    msg2.post_cfg = (1<<2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = dcache0;
    msg2.dest = membus;
    msg2.cmd = readExreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<9);
    msg3.src = membus;
    msg3.dest = icache1;
    msg3.cmd = readExreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    /**
     message_t msg4;
     msg4.pre_cfg = (1<<6);
     msg4.post_cfg = (1<<9);
     msg4.src = dcache1;
     msg4.dest = cpu1;
     msg4.cmd = readExreq;
     msg4.addr = NDEF;
     lpn->insert_msg(msg4);
     **/
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1<<10);
    msg5.src = membus;
    msg5.dest = dcache1;
    msg5.cmd = readExreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    /**
     message_t msg6;
     msg6.pre_cfg = (1<<7);
     msg6.post_cfg = (1<<10);
     msg6.src = icache1;
     msg6.dest = cpu1;
     msg6.cmd = readExreq;
     msg6.addr = NDEF;
     lpn->insert_msg(msg6);
     **/
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 11);
    msg7.src = membus;
    msg7.dest = icache0;
    msg7.cmd = readExreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    /**
     message_t msg8;
     msg8.pre_cfg = (1<<8);
     msg8.post_cfg = (1<<11);
     msg8.src = dcache0;
     msg8.dest = cpu0;
     msg8.cmd = readExreq;
     msg8.addr = NDEF;
     lpn->insert_msg(msg8);
     **/
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1<<12);
    msg9.src = membus;
    msg9.dest = mem;
    msg9.cmd = readExreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
    
    message_t msg11;
    msg11.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg11.post_cfg = (1 << 14);
    msg11.src = mem;
    msg11.dest = membus;
    msg11.cmd = readExres;
    msg11.addr = NDEF;
    lpn->insert_msg(msg11);
    
    message_t msg12;
    msg12.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg12.post_cfg = (1 << 14);
    msg12.src = icache1;
    msg12.dest = membus;
    msg12.cmd = readExres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14);
    msg13.post_cfg = (1 << 16);
    msg13.src = membus;
    msg13.dest = dcache0;
    msg13.cmd = readExres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    /**
     message_t msg14;
     msg14.pre_cfg = (1<<15);
     msg14.post_cfg = (1<<16);
     msg14.src = icache0;
     msg14.dest = cpu0;
     msg14.cmd = writeres;
     msg14.addr = NDEF;
     lpn->insert_msg(msg14);
     
     message_t msg15;
     msg15.pre_cfg = (1<<1);
     msg15.post_cfg = (1<<17);
     msg15.src = icache0;
     msg15.dest = cpu0;
     msg15.cmd = writeres;
     msg15.addr = NDEF;
     lpn->insert_msg(msg15);
     **/
    message_t msg22;
    msg22.pre_cfg = (1<<1);
    msg22.post_cfg = (1<<18)|(1 << 19)|(1 << 20)|(1 << 21);
    msg22.src = icache0;
    msg22.dest = membus;
    msg22.cmd = Upgradereq;
    msg22.addr = NDEF;
    lpn->insert_msg(msg22);
    
    message_t msg23;
    msg23.pre_cfg = (1<<18);
    msg23.post_cfg = (1<<25);
    msg23.src = membus;
    msg23.dest = icache1;
    msg23.cmd = Upgradereq;
    msg23.addr = NDEF;
    lpn->insert_msg(msg23);
    
    
    /**
     message_t msg24;
     msg24.pre_cfg = (1<<22);
     msg24.post_cfg = (1<<25);
     msg24.src = icache1;
     msg24.dest = cpu1;
     msg24.cmd = Upgradereq;
     msg24.addr = NDEF;
     lpn->insert_msg(msg24);
     **/
    message_t msg25;
    msg25.pre_cfg = (1<<19);
    msg25.post_cfg = (1<<26);
    msg25.src = membus;
    msg25.dest = icache1;
    msg25.cmd = Upgradereq;
    msg25.addr = NDEF;
    lpn->insert_msg(msg25);
    /**
     message_t msg26;
     msg26.pre_cfg = (1<<23);
     msg26.post_cfg = (1<<26);
     msg26.src = icache1;
     msg26.dest = cpu1;
     msg26.cmd = Upgradereq;
     msg26.addr = NDEF;
     lpn->insert_msg(msg26);
     **/
    message_t msg27;
    msg27.pre_cfg = (1<<20);
    msg27.post_cfg = (1 << 27);
    msg27.src = membus;
    msg27.dest = icache0;
    msg27.cmd = Upgradereq;
    msg27.addr = NDEF;
    lpn->insert_msg(msg27);
    
    /**
     message_t msg28;
     msg28.pre_cfg = (1<<24);
     msg28.post_cfg = (1<<27);
     msg28.src = icache0;
     msg28.dest = cpu0;
     msg28.cmd = Upgradereq;
     msg28.addr = NDEF;
     lpn->insert_msg(msg28);
     **/
    
    message_t msg29;
    msg29.pre_cfg = (1<<21);
    msg29.post_cfg = (1<<28);
    msg29.src = membus;
    msg29.dest = mem;
    msg29.cmd = Upgradereq;
    msg29.addr = NDEF;
    lpn->insert_msg(msg29);
    
    
    message_t msg31;
    msg31.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg31.post_cfg = (1 << 29);
    msg31.src = mem;
    msg31.dest = membus;
    msg31.cmd = Upgraderes;
    msg31.addr = NDEF;
    lpn->insert_msg(msg31);
    
    message_t msg32;
    msg32.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg32.post_cfg = (1 << 29);
    msg32.src = icache1;
    msg32.dest = membus;
    msg32.cmd = Upgraderes;
    msg32.addr = NDEF;
    lpn->insert_msg(msg32);
    
    message_t msg33;
    msg33.pre_cfg = (1<29);
    msg33.post_cfg = (1 << 31);
    msg33.src = membus;
    msg33.dest = icache0;
    msg33.cmd = Upgraderes;
    msg32.addr = NDEF;
    lpn->insert_msg(msg33);
    /**
     message_t msg34;
     msg34.pre_cfg = (1<30);
     msg34.post_cfg = (1<<31);
     msg34.src = icache0;
     msg34.dest = cpu0;
     msg34.cmd = writeres;
     msg34.addr = NDEF;
     lpn->insert_msg(msg34);
     **/
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
    
}
lpn_t* build_us_mem_rd_flow_v1(void)
{
    
    
    
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu1 write*******");
    /**
     message_t msg1;
     msg1.pre_cfg = (1<<0);
     msg1.post_cfg = (1 << 1);
     msg1.src = cpu1;
     msg1.dest = icache1;
     msg1.cmd = writereq;
     msg1.addr = NDEF;
     lpn->insert_msg(msg1);
     **/
    
    message_t msg2;
    msg2.pre_cfg = (1<<0);
    msg2.post_cfg = (1<<2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = dcache1;
    msg2.dest = membus;
    msg2.cmd = readExreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<9);
    msg3.src = membus;
    msg3.dest = icache0;
    msg3.cmd = readExreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    /**
     message_t msg4;
     msg4.pre_cfg = (1<<6);
     msg4.post_cfg = (1<<9);
     msg4.src = dcache0;
     msg4.dest = cpu0;
     msg4.cmd = readExreq;
     msg4.addr = NDEF;
     lpn->insert_msg(msg4);
     **/
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1<<10);
    msg5.src = membus;
    msg5.dest = dcache0;
    msg5.cmd = readExreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    /**
     message_t msg6;
     msg6.pre_cfg = (1<<7);
     msg6.post_cfg = (1<<10);
     msg6.src = icache0;
     msg6.dest = cpu0;
     msg6.cmd = readExreq;
     msg6.addr = NDEF;
     lpn->insert_msg(msg6);
     **/
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 11);
    msg7.src = membus;
    msg7.dest = icache1;
    msg7.cmd = readExreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    /**
     message_t msg8;
     msg8.pre_cfg = (1<<8);
     msg8.post_cfg = (1<<11);
     msg8.src = dcache1;
     msg8.dest = cpu1;
     msg8.cmd = readExreq;
     msg8.addr = NDEF;
     lpn->insert_msg(msg8);
     **/
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1<<12);
    msg9.src = membus;
    msg9.dest = mem;
    msg9.cmd = readExreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
    
    message_t msg11;
    msg11.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg11.post_cfg = (1 << 14);
    msg11.src = mem;
    msg11.dest = membus;
    msg11.cmd = readExres;
    msg11.addr = NDEF;
    lpn->insert_msg(msg11);
    
    message_t msg12;
    msg12.pre_cfg = (1<<9)|(1<<10)|(1<<11)|(1<<12);
    msg12.post_cfg = (1 << 14);
    msg12.src = icache0;
    msg12.dest = membus;
    msg12.cmd = readExres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14);
    msg13.post_cfg = (1 << 16);
    msg13.src = membus;
    msg13.dest = dcache1;
    msg13.cmd = readExres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    /**
     message_t msg14;
     msg14.pre_cfg = (1<<15);
     msg14.post_cfg = (1<<16);
     msg14.src = icache1;
     msg14.dest = cpu1;
     msg14.cmd = writeres;
     msg14.addr = NDEF;
     lpn->insert_msg(msg14);
     
     message_t msg15;
     msg15.pre_cfg = (1<<1);
     msg15.post_cfg = (1<<17);
     msg15.src = icache1;
     msg15.dest = cpu1;
     msg15.cmd = writeres;
     msg15.addr = NDEF;
     lpn->insert_msg(msg15);
     **/
    message_t msg22;
    msg22.pre_cfg = (1<<1);
    msg22.post_cfg = (1<<18)|(1 << 19)|(1 << 20)|(1 << 21);
    msg22.src = icache1;
    msg22.dest = membus;
    msg22.cmd = Upgradereq;
    msg22.addr = NDEF;
    lpn->insert_msg(msg22);
    
    message_t msg23;
    msg23.pre_cfg = (1<<18);
    msg23.post_cfg = (1<<25);
    msg23.src = membus;
    msg23.dest = icache0;
    msg23.cmd = Upgradereq;
    msg23.addr = NDEF;
    lpn->insert_msg(msg23);
    
    
    /**
     message_t msg24;
     msg24.pre_cfg = (1<<22);
     msg24.post_cfg = (1<<25);
     msg24.src = icache0;
     msg24.dest = cpu0;
     msg24.cmd = Upgradereq;
     msg24.addr = NDEF;
     lpn->insert_msg(msg24);
     **/
    message_t msg25;
    msg25.pre_cfg = (1<<19);
    msg25.post_cfg = (1<<26);
    msg25.src = membus;
    msg25.dest = icache0;
    msg25.cmd = Upgradereq;
    msg25.addr = NDEF;
    lpn->insert_msg(msg25);
    /**
     message_t msg26;
     msg26.pre_cfg = (1<<23);
     msg26.post_cfg = (1<<26);
     msg26.src = icache0;
     msg26.dest = cpu0;
     msg26.cmd = Upgradereq;
     msg26.addr = NDEF;
     lpn->insert_msg(msg26);
     **/
    message_t msg27;
    msg27.pre_cfg = (1<<20);
    msg27.post_cfg = (1 << 27);
    msg27.src = membus;
    msg27.dest = icache1;
    msg27.cmd = Upgradereq;
    msg27.addr = NDEF;
    lpn->insert_msg(msg27);
    
    /**
     message_t msg28;
     msg28.pre_cfg = (1<<24);
     msg28.post_cfg = (1<<27);
     msg28.src = icache1;
     msg28.dest = cpu1;
     msg28.cmd = Upgradereq;
     msg28.addr = NDEF;
     lpn->insert_msg(msg28);
     **/
    
    message_t msg29;
    msg29.pre_cfg = (1<<21);
    msg29.post_cfg = (1<<28);
    msg29.src = membus;
    msg29.dest = mem;
    msg29.cmd = Upgradereq;
    msg29.addr = NDEF;
    lpn->insert_msg(msg29);
    
    
    message_t msg31;
    msg31.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg31.post_cfg = (1 << 29);
    msg31.src = mem;
    msg31.dest = membus;
    msg31.cmd = Upgraderes;
    msg31.addr = NDEF;
    lpn->insert_msg(msg31);
    
    message_t msg32;
    msg32.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg32.post_cfg = (1 << 29);
    msg32.src = icache0;
    msg32.dest = membus;
    msg32.cmd = Upgraderes;
    msg32.addr = NDEF;
    lpn->insert_msg(msg32);
    
    message_t msg33;
    msg33.pre_cfg = (1<29);
    msg33.post_cfg = (1 << 31);
    msg33.src = membus;
    msg33.dest = icache1;
    msg33.cmd = Upgraderes;
    msg32.addr = NDEF;
    lpn->insert_msg(msg33);
    /**
     message_t msg34;
     msg34.pre_cfg = (1<30);
     msg34.post_cfg = (1<<31);
     msg34.src = icache1;
     msg34.dest = cpu1;
     msg34.cmd = writeres;
     msg34.addr = NDEF;
     lpn->insert_msg(msg34);
     **/
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
    
}

lpn_t* build_cpu0_read(void) {
    
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu0 read*******");
    
    /**
     message_t msg1;
     msg1.pre_cfg = (1<<0);
     msg1.post_cfg = (1 << 1);
     msg1.src = cpu0;
     msg1.dest = icache0;
     msg1.cmd = readreq;
     msg1.addr = NDEF;
     lpn->insert_msg(msg1);
     **/
    
    message_t msg2;
    msg2.pre_cfg = (1<<0);
    msg2.post_cfg = (1<<2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = dcache0;
    msg2.dest = membus;
    msg2.cmd = storeCondreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<9);
    msg3.src = membus;
    msg3.dest = icache1;
    msg3.cmd = storeCondreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    /**
     message_t msg4;
     msg4.pre_cfg = (1<<6);
     msg4.post_cfg = (1<<9);
     msg4.src = dcache1;
     msg4.dest = cpu1;
     msg4.cmd = storeCondreq;
     msg4.addr = NDEF;
     lpn->insert_msg(msg4);
     **/
    
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1<<10);
    msg5.src = membus;
    msg5.dest = dcache1;
    msg5.cmd = storeCondreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    /**
     message_t msg6;
     msg6.pre_cfg = (1<<7);
     msg6.post_cfg = (1<<10);
     msg6.src = icache1;
     msg6.dest = cpu1;
     msg6.cmd = storeCondreq;
     msg6.addr = NDEF;
     lpn->insert_msg(msg6);
     **/
    
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 11);
    msg7.src = membus;
    msg7.dest = icache0;
    msg7.cmd = storeCondreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    /**
     message_t msg8;
     msg8.pre_cfg = (1<<8);
     msg8.post_cfg = (1<<11);
     msg8.src = dcache0;
     msg8.dest = cpu0;
     msg8.cmd = storeCondreq;
     msg8.addr = NDEF;
     lpn->insert_msg(msg8);
     **/
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1<<12);
    msg9.src = membus;
    msg9.dest = mem;
    msg9.cmd = storeCondreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
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
    msg12.src = icache1;
    msg12.dest = membus;
    msg12.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14);
    msg13.post_cfg = (1 << 16);
    msg13.src = membus;
    msg13.dest = dcache0;
    msg13.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    
    /**
     message_t msg14;
     msg14.pre_cfg = (1<<15);
     msg14.post_cfg = (1 << 16);
     msg14.src = icache0;
     msg14.dest = cpu0;
     msg14.cmd = readres;
     msg14.addr = NDEF;
     lpn->insert_msg(msg14);
     
     message_t msg15;
     msg15.pre_cfg = (1<<1);
     msg15.post_cfg = (1 << 17);
     msg15.src = icache0;
     msg15.dest = cpu0;
     msg15.cmd = readres;
     msg15.addr = NDEF;
     lpn->insert_msg(msg15);
     **/
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}
lpn_t* build_cpu1_read(void) {
    
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu1 read*******");
    /**
     message_t msg1;
     msg1.pre_cfg = (1<<0);
     msg1.post_cfg = (1 << 1);
     msg1.src = cpu1;
     msg1.dest = icache1;
     msg1.cmd = readreq;
     msg1.addr = NDEF;
     lpn->insert_msg(msg1);
     **/
    
    message_t msg2;
    msg2.pre_cfg = (1<<0);
    msg2.post_cfg = (1<<2)|(1 << 4)|(1 << 3)|(1 << 5);
    msg2.src = dcache1;
    msg2.dest = membus;
    msg2.cmd = storeCondreq;
    msg2.addr = NDEF;
    lpn->insert_msg(msg2);
    
    message_t msg3;
    msg3.pre_cfg = (1<<2);
    msg3.post_cfg = (1<<9);
    msg3.src = membus;
    msg3.dest = icache0;
    msg3.cmd = storeCondreq;
    msg3.addr = NDEF;
    lpn->insert_msg(msg3);
    
    /**
     message_t msg4;
     msg4.pre_cfg = (1<<6);
     msg4.post_cfg = (1<<9);
     msg4.src = dcache0;
     msg4.dest = cpu0;
     msg4.cmd = storeCondreq;
     msg4.addr = NDEF;
     lpn->insert_msg(msg4);
     **/
    
    message_t msg5;
    msg5.pre_cfg = (1<<3);
    msg5.post_cfg = (1<<10);
    msg5.src = membus;
    msg5.dest = dcache0;
    msg5.cmd = storeCondreq;
    msg5.addr = NDEF;
    lpn->insert_msg(msg5);
    
    /**
     message_t msg6;
     msg6.pre_cfg = (1<<7);
     msg6.post_cfg = (1<<10);
     msg6.src = icache0;
     msg6.dest = cpu0;
     msg6.cmd = storeCondreq;
     msg6.addr = NDEF;
     lpn->insert_msg(msg6);
     **/
    
    message_t msg7;
    msg7.pre_cfg = (1<<4);
    msg7.post_cfg = (1 << 11);
    msg7.src = membus;
    msg7.dest = icache1;
    msg7.cmd = storeCondreq;
    msg7.addr = NDEF;
    lpn->insert_msg(msg7);
    
    /**
     message_t msg8;
     msg8.pre_cfg = (1<<8);
     msg8.post_cfg = (1<<11);
     msg8.src = dcache1;
     msg8.dest = cpu1;
     msg8.cmd = storeCondreq;
     msg8.addr = NDEF;
     lpn->insert_msg(msg8);
     **/
    
    message_t msg9;
    msg9.pre_cfg = (1<<5);
    msg9.post_cfg = (1<<12);
    msg9.src = membus;
    msg9.dest = mem;
    msg9.cmd = storeCondreq;
    msg9.addr = NDEF;
    lpn->insert_msg(msg9);
    
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
    msg12.src = icache0;
    msg12.dest = membus;
    msg12.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg12);
    
    message_t msg13;
    msg13.pre_cfg = (1<<14);
    msg13.post_cfg = (1 << 16);
    msg13.src = membus;
    msg13.dest = dcache1;
    msg13.cmd = readres;
    msg12.addr = NDEF;
    lpn->insert_msg(msg13);
    
    /**
     message_t msg14;
     msg14.pre_cfg = (1<<15);
     msg14.post_cfg = (1 << 16);
     msg14.src = icache1;
     msg14.dest = cpu1;
     msg14.cmd = readres;
     msg14.addr = NDEF;
     lpn->insert_msg(msg14);
     
     message_t msg15;
     msg15.pre_cfg = (1<<1);
     msg15.post_cfg = (1 << 17);
     msg15.src = icache1;
     msg15.dest = cpu1;
     msg15.cmd = readres;
     msg15.addr = NDEF;
     lpn->insert_msg(msg15);
     **/
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}

lpn_t* build_cpu0_dread(void){
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu0 read to dcache*******");
    /**
     message_t msg1;
     msg1.pre_cfg = (1<<0);
     msg1.post_cfg = (1 << 1);
     msg1.src = cpu0;
     msg1.dest = dcache0;
     msg1.cmd = readreq;
     msg1.addr = NDEF;
     lpn->insert_msg(msg1);
     **/
    
    message_t msg22;
    msg22.pre_cfg = (1<<0);
    msg22.post_cfg = (1<<19)|(1 << 18)|(1 <<20)|(1 << 21);
    msg22.src = icache0;
    msg22.dest = membus;
    msg22.cmd = loadLockedreq;
    msg22.addr = NDEF;
    lpn->insert_msg(msg22);
    
    message_t msg23;
    msg23.pre_cfg = (1<<18);
    msg23.post_cfg = (1<<25);
    msg23.src = membus;
    msg23.dest = icache1;
    msg23.cmd = loadLockedreq;
    msg23.addr = NDEF;
    lpn->insert_msg(msg23);
    
    /**
     message_t msg24;
     msg24.pre_cfg = (1<<22);
     msg24.post_cfg = (1<<25);
     msg24.src = dcache1;
     msg24.dest = cpu1;
     msg24.cmd = loadLockedreq;
     msg24.addr = NDEF;
     lpn->insert_msg(msg24);
     **/
    message_t msg25;
    msg25.pre_cfg = (1<<19);
    msg25.post_cfg = (1<<26);
    msg25.src = membus;
    msg25.dest = dcache1;
    msg25.cmd = loadLockedreq;
    msg25.addr = NDEF;
    lpn->insert_msg(msg25);
    /**
     message_t msg26;
     msg26.pre_cfg = (1<<23);
     msg26.post_cfg = (1<<26);
     msg26.src = icache1;
     msg26.dest = cpu1;
     msg26.cmd = loadLockedreq;
     msg26.addr = NDEF;
     lpn->insert_msg(msg26);
     **/
    message_t msg27;
    msg27.pre_cfg = (1<<20);
    msg27.post_cfg = (1 << 27);
    msg27.src = membus;
    msg27.dest = dcache0;
    msg27.cmd = loadLockedreq;
    msg27.addr = NDEF;
    lpn->insert_msg(msg27);
    
    /**
     message_t msg28;
     msg28.pre_cfg = (1<<24);
     msg28.post_cfg = (1<<27);
     msg28.src = icache0;
     msg28.dest = cpu0;
     msg28.cmd = loadLockedreq;
     msg28.addr = NDEF;
     lpn->insert_msg(msg28);
     **/
    
    message_t msg29;
    msg29.pre_cfg = (1<<21);
    msg29.post_cfg = (1<<28);
    msg29.src = membus;
    msg29.dest = mem;
    msg29.cmd = loadLockedreq;
    msg29.addr = NDEF;
    lpn->insert_msg(msg29);
    
    message_t msg31;
    msg31.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg31.post_cfg = (1 << 29);
    msg31.src = mem;
    msg31.dest = membus;
    msg31.cmd = readres;
    msg31.addr = NDEF;
    lpn->insert_msg(msg31);
    
    message_t msg32;
    msg32.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg32.post_cfg = (1 << 29);
    msg32.src = icache1;
    msg32.dest = membus;
    msg32.cmd = readres;
    msg32.addr = NDEF;
    lpn->insert_msg(msg32);
    
    message_t msg33;
    msg33.pre_cfg = (1<<29);
    msg33.post_cfg = (1 << 31);
    msg33.src = membus;
    msg33.dest = icache0;
    msg33.cmd = readres;
    msg32.addr = NDEF;
    lpn->insert_msg(msg33);
    
    /**
     message_t msg34;
     msg34.pre_cfg = (1<<30);
     msg34.post_cfg = (1 << 31);
     msg34.src = dcache0;
     msg34.dest = cpu0;
     msg34.cmd = readres;
     msg34.addr = NDEF;
     lpn->insert_msg(msg34);
     
     
     message_t msg14;
     msg14.pre_cfg = (1<<1);
     msg14.post_cfg = (1 << 17);
     msg14.src = dcache0;
     msg14.dest = cpu0;
     msg14.cmd = readres;
     msg14.addr = NDEF;
     lpn->insert_msg(msg14);
     
     **/
    lpn->set_init_cfg(1<<0);
    return lpn;
}
lpn_t* build_cpu1_dread(void){
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu1 read to dcache*******");
    /**
     message_t msg1;
     msg1.pre_cfg = (1<<0);
     msg1.post_cfg = (1 << 1);
     msg1.src = cpu1;
     msg1.dest = dcache1;
     msg1.cmd = readreq;
     msg1.addr = NDEF;
     lpn->insert_msg(msg1);
     **/
    
    message_t msg22;
    msg22.pre_cfg = (1<<0);
    msg22.post_cfg = (1<<19)|(1 << 18)|(1 <<20)|(1 << 21);
    msg22.src = icache1;
    msg22.dest = membus;
    msg22.cmd = loadLockedreq;
    msg22.addr = NDEF;
    lpn->insert_msg(msg22);
    
    message_t msg23;
    msg23.pre_cfg = (1<<18);
    msg23.post_cfg = (1<<25);
    msg23.src = membus;
    msg23.dest = icache0;
    msg23.cmd = loadLockedreq;
    msg23.addr = NDEF;
    lpn->insert_msg(msg23);
    
    /**
     message_t msg24;
     msg24.pre_cfg = (1<<22);
     msg24.post_cfg = (1<<25);
     msg24.src = dcache0;
     msg24.dest = cpu0;
     msg24.cmd = loadLockedreq;
     msg24.addr = NDEF;
     lpn->insert_msg(msg24);
     **/
    message_t msg25;
    msg25.pre_cfg = (1<<19);
    msg25.post_cfg = (1<<26);
    msg25.src = membus;
    msg25.dest = dcache0;
    msg25.cmd = loadLockedreq;
    msg25.addr = NDEF;
    lpn->insert_msg(msg25);
    /**
     message_t msg26;
     msg26.pre_cfg = (1<<23);
     msg26.post_cfg = (1<<26);
     msg26.src = icache0;
     msg26.dest = cpu0;
     msg26.cmd = loadLockedreq;
     msg26.addr = NDEF;
     lpn->insert_msg(msg26);
     **/
    message_t msg27;
    msg27.pre_cfg = (1<<20);
    msg27.post_cfg = (1 << 27);
    msg27.src = membus;
    msg27.dest = dcache1;
    msg27.cmd = loadLockedreq;
    msg27.addr = NDEF;
    lpn->insert_msg(msg27);
    
    /**
     message_t msg28;
     msg28.pre_cfg = (1<<24);
     msg28.post_cfg = (1<<27);
     msg28.src = icache1;
     msg28.dest = cpu1;
     msg28.cmd = loadLockedreq;
     msg28.addr = NDEF;
     lpn->insert_msg(msg28);
     **/
    
    message_t msg29;
    msg29.pre_cfg = (1<<21);
    msg29.post_cfg = (1<<28);
    msg29.src = membus;
    msg29.dest = mem;
    msg29.cmd = loadLockedreq;
    msg29.addr = NDEF;
    lpn->insert_msg(msg29);
    
    message_t msg31;
    msg31.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg31.post_cfg = (1 << 29);
    msg31.src = mem;
    msg31.dest = membus;
    msg31.cmd = readres;
    msg31.addr = NDEF;
    lpn->insert_msg(msg31);
    
    message_t msg32;
    msg32.pre_cfg = (1<<25)|(1<<26)|(1<<27)|(1<<28);
    msg32.post_cfg = (1 << 29);
    msg32.src = icache0;
    msg32.dest = membus;
    msg32.cmd = readres;
    msg32.addr = NDEF;
    lpn->insert_msg(msg32);
    
    message_t msg33;
    msg33.pre_cfg = (1<<29);
    msg33.post_cfg = (1 << 31);
    msg33.src = membus;
    msg33.dest = icache1;
    msg33.cmd = readres;
    msg32.addr = NDEF;
    lpn->insert_msg(msg33);
    
    /**
     message_t msg34;
     msg34.pre_cfg = (1<<30);
     msg34.post_cfg = (1 << 31);
     msg34.src = dcache1;
     msg34.dest = cpu1;
     msg34.cmd = readres;
     msg34.addr = NDEF;
     lpn->insert_msg(msg34);
     
     
     message_t msg14;
     msg14.pre_cfg = (1<<1);
     msg14.post_cfg = (1 << 17);
     msg14.src = dcache1;
     msg14.dest = cpu1;
     msg14.cmd = readres;
     msg14.addr = NDEF;
     lpn->insert_msg(msg14);
     
     **/
    lpn->set_init_cfg(1<<0);
    return lpn;
}
lpn_t* build_cpu0_storefail(void){
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu0 storeCondfail******");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1);
    msg1.src = cpu0;
    msg1.dest = icache0;
    msg1.cmd = StoreCondFailreq;
    msg1.addr = NDEF;
    lpn->insert_msg(msg1);
    
    
    message_t msg15;
    msg15.pre_cfg = (1<<1);
    msg15.post_cfg = (1 << 17);
    msg15.src = icache0;
    msg15.dest = cpu0;
    msg15.cmd = readres;
    msg15.addr = NDEF;
    lpn->insert_msg(msg15);
    
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}

lpn_t* build_cpu1_storefail(void){
    lpn_t* lpn = new lpn_t;
    
    lpn->set_flow_name("****cpu1 storeCondfail******");
    
    message_t msg1;
    msg1.pre_cfg = (1<<0);
    msg1.post_cfg = (1 << 1);
    msg1.src = cpu1;
    msg1.dest = icache1;
    msg1.cmd = StoreCondFailreq;
    msg1.addr = NDEF;
    lpn->insert_msg(msg1);
    
    
    message_t msg15;
    msg15.pre_cfg = (1<<1);
    msg15.post_cfg = (1 << 17);
    msg15.src = icache1;
    msg15.dest = cpu1;
    msg15.cmd = readres;
    msg15.addr = NDEF;
    lpn->insert_msg(msg15);
    
    
    lpn->set_init_cfg(1<<0);
    
    return lpn;
}
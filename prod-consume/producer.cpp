#include "write_if.cpp"

class producer : public sc_module
{ 
public:
	sc_port<write_if> out; //Producer has a port with write interface 
	
	SC_HAS_PROCESS(producer);
	producer(sc_module_name name) : sc_module(name) {
		SC_THREAD(main_action); 
	}

	void main_action() {
		const char *str = "Hello How are you! see what SystemC can do for you today!\n";
		while (*str) {
            char c = *str;
            str++;
			out->write(c);
            cout << "write " << c << endl;
        }
	}
};
#include "read_if.cpp"

class consumer : public sc_module
{ 
public:
	sc_port<read_if> in; // Consumer has a port with read interface 
	
	//SC_HAS_PROCESS(consumer);

	//consumer(sc_module_name name) : sc_module(name) {
	SC_CTOR(consumer) {
        SC_THREAD(main_action);
	}

	void main_action( ) {
		char c;

		while (true) {
			in->read(c);
			cout << "read " << c << endl;
			/*
			if (in->num_available() == 1) 
				cout << "<1>" << flush;

			if (in->num_available() == 9)
				cout << "<9>" << flush;
             */
		}
	}
};
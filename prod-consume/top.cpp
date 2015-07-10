#include "producer.cpp"
#include "consumer.cpp"
#include "fifo.cpp"

/*
class TOP : public sc_module
{ 
public:
	myfifo *fifo_inst;
	producer *prod_inst; 
	consumer *cons_inst;
	
	TOP(sc_module_name name) : sc_module(name) {
		fifo_inst = new myfifo("Fifo1"); //FIFO Channel Instantiation
		
		prod_inst = new producer("Producer1"); //Producer Instantiation 
		prod_inst->out(*fifo_inst); //Port Binding

		cons_inst = new consumer("Consumer1"); //Consumer Instantiation
		cons_inst->in(*fifo_inst); //Port Binding
    }
};
*/

int sc_main(int argc, char* argv[])
{
    myfifo *fifo_inst;
	producer *prod_inst;
	consumer *cons_inst;
	
		fifo_inst = new myfifo("Fifo1"); //FIFO Channel Instantiation
		
		prod_inst = new producer("Producer1"); //Producer Instantiation
		prod_inst->out(*fifo_inst); //Port Binding
        
		cons_inst = new consumer("Consumer1"); //Consumer Instantiation
		cons_inst->in(*fifo_inst); //Port Binding

	
    sc_start();
	return 0;
}

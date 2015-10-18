#include <iostream>
#include "worker.h"

using namespace std;

void *worker::process(void * W){
	worker *Worker = (worker *)W;
	cout << (char *)Worker->msg.getMessage() << endl;
}

worker::worker(message *MSG, UDPsocket server_socket):msg(*MSG), socket(server_socket){

}

worker::~worker(){

}

void worker::deploy(){
    if(pthread_create(&thread_id, NULL, process, this) < 0) throw("Error Creating Thread");
}

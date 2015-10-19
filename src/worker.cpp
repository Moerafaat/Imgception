#include <iostream>
#include "worker.h"

using namespace std;

void *worker::process(void * W){
	worker *Worker = (worker *)W;
	cout << (char *)Worker->msg.getMessage() << endl;
}

worker::worker(message *MSG, UDPsocket server_socket):msg(*MSG), socket(server_socket){
	*(unsigned int *)ip_port = htonl(0x7F000001);
	*(unsigned short *)(ip_port + 4) = htons(server_socket.getMyPort());
}

worker::~worker(){

}

void worker::deploy(){
    if(pthread_create(&thread_id, NULL, process, this) < 0) throw("Error Creating Thread");
}

const char *worker::getIPandPort() const{
	return ip_port;
}

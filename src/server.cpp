#include <iostream>
#include "server.h"

using namespace std;

server::server(const char *, short my_port): server_socket(my_port){ // Construct server socket.

}

server::~server(){

}

void server::listen(){
	char *buffer;
	int size;
	while(1){ // Continuously listening for incoming client data.
		buffer = server_socket.syncRead(size);
		message msg(buffer, size);
		workers.push_back(new worker(&msg, server_socket.clone())); // Spawn a worker in a vector of workers to handle the client.
		workers.back()->deploy(); // Deploy the thread.
	}
}

// Threads/Workers mechanism.
// Workers are still not yet deleted (Intentional until it is discussed in class).

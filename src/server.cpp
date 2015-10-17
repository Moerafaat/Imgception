#include <iostream>
#include "server.h"

using namespace std;

server::server(const char *, int my_port): server_socket(my_port){

}

server::~server(){

}

void server::listen(){
	char *buffer;
	int size;
	while(1){
		buffer = server_socket.syncRead(size);
		cout << buffer << endl;
		delete []buffer;
	}
}
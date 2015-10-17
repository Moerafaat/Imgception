#include "client.h"

client::client(const char *hostname, int peer_port): client_socket(hostname, peer_port){

}

client::~client(){

}

message *client::execute(const message *msg){
	int size = client_socket.asyncWrite((const char*)msg->getMessage(), msg->getMessageSize());
	return nullptr; // Temporary.
}
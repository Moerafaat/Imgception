#include "client.h"

client::client(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port, true){
	// Resovles the host first, then constructs the client socket.
}

client::~client(){

}

message *client::execute(const message *msg){
	int size = client_socket.asyncWrite((const char*)msg->getMessage(), msg->getMessageSize());
	return nullptr;
}

// Send Reply.
// Port to Worker.

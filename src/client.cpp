#include <iostream>
#include "client.h"

using namespace std;

client::client(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port){
	// Resovles the host first, then constructs the client socket.
}

client::~client(){

}

message *client::execute(const message *msg){
	unsigned short key = htons(rand() + client_socket.getMyPort());
	cout << "Client Port: " << client_socket.getMyPort() << "\t" << "Client Key: " << ntohs(key) << endl;
	int size = client_socket.asyncWrite((char *)&key, sizeof(key)); // Send to server.
	//char *buffer = client_socket.syncRead(size); // Blocking for reply for reply from server.

	//unsigned int worker_ip = ntohl(*(unsigned int*)buffer);
	//unsigned short worker_port = ntohs(*(unsigned short*)(buffer + 4));
	//cout << "IP: " << worker_ip << " Port: " << worker_port << endl;
	return nullptr;
}

// Send Reply.
// Port to Worker.
// Handle spamming.

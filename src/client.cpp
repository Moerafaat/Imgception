#include <iostream>
#include <string>
#include "client.h"

using namespace std;

client::client(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port){
	// Resovles the host first, then constructs the client socket.
	server_ip = client_socket.getPeerIP();
	server_port = peer_port;
	my_key = rand() + client_socket.getMyPort();
}

client::~client(){

}

int client::connect(){
	unsigned int key = htonl(my_key);
	cout << "Client Port: " << client_socket.getMyPort() << "\t" << "Client Key: " << ntohl(key) << endl;

	char send_str[1 + sizeof(key)];
	send_str[0] = 1; // quit = 0. msg = 1.
	memcpy(send_str + 1, (char *)&key, sizeof(key));

	int size = client_socket.asyncWrite(send_str, sizeof(send_str)); // Send to server.

	char *buf = client_socket.syncRead(size); // Blocking for reply for reply from server.
	cout << "Confirmation from worker: " << ntohl(*(unsigned int*)buf) << endl;
	client_socket.bindPeer(client_socket.getPeerIP(), client_socket.getPeerPort());
	return 0;
}

void client::disconnect(){
	client_socket.releasePeer(server_ip, server_port);
}

message *client::execute(const message *msg){
	string tstr;
	tstr.push_back(1);
	tstr += *(string*)msg->getMessage();
	int size = client_socket.asyncWrite(tstr.c_str(), tstr.size() + 1);

	return nullptr;
}

// Send Reply.
// Port to Worker.
// Handle spamming.

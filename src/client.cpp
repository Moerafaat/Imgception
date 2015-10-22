#include <iostream>
#include <string>
#include "client.h"

using namespace std;

client::client(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port){
	// Resovles the host first, then constructs the client socket.
	server_ip = client_socket.getPeerIP();
	server_port = peer_port;
}

client::~client(){

}

message *client::execute(const message *msg){
	unsigned int key = htonl(rand() + client_socket.getMyPort());
	cout << "Client Port: " << client_socket.getMyPort() << "\t" << "Client Key: " << ntohl(key) << endl;

	int size = client_socket.asyncWrite((char *)&key, sizeof(key)); // Send to server.

	char *buf = client_socket.syncRead(size); // Blocking for reply for reply from server.
	cout << "Confirmation from worker: " << ntohl(*(unsigned int*)buf) << endl;
	client_socket.bindPeer(client_socket.getPeerIP(), client_socket.getPeerPort());

	string str;
	while(getline(cin, str)) {
		size = client_socket.asyncWrite(str.c_str(), str.size() + 1);
	}

	client_socket.releasePeer(server_ip, server_port);
	return nullptr;
}

// Send Reply.
// Port to Worker.
// Handle spamming.

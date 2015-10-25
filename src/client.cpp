#include <iostream>
#include <string>
#include "client.h"

using namespace std;

client::client(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port){
	// Resovles the host first, then constructs the client socket.
	server_ip = client_socket.getPeerIP();
	server_port = peer_port;
	my_key = rand() + client_socket.getMyPort();
	nTries = 3;
}

client::~client(){

}

int client::connect(){
	client_socket.setReadTimeout(5000);
	unsigned int key = htonl(my_key);
	cout << "Client Port: " << client_socket.getMyPort() << "\t" << "Client Key: " << ntohl(key) << endl;
	unsigned int i;
	char *buf;
	for(i = 0; i < nTries; i++){	//*****************Might be organized in a better way.
		char send_str[1 + sizeof(key)];
		send_str[0] = 1; // quit = 0. msg = 1.
		memcpy(send_str + 1, (char *)&key, sizeof(key));
		int size = client_socket.asyncWrite(send_str, sizeof(send_str)); // Send to server.

		buf = client_socket.syncRead(size); // Blocking for reply for reply from server.
		if(buf != nullptr) break;
		cout << "Trial " << i + 1 << " failed. Retrying..." << endl;
	}
	if(i == nTries) return -1;
	cout << "Confirmation from worker: " << ntohl(*(unsigned int*)buf) << endl;
	client_socket.bindPeer(client_socket.getPeerIP(), client_socket.getPeerPort());
	delete [] buf;
	return 0;
}

void client::disconnect(){
	client_socket.releasePeer(server_ip, server_port);
}

void client::setTries(const unsigned int n){
	nTries = n;
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

#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "message.h"
#include "UDPsocket.h"
#include "worker.h"

class server{
	server() = delete;
	server(const server&) = delete;
	server(server&&) = delete;
private:
	UDPsocket server_socket;
	std::vector<worker *> workers;

public:
	server(const char *, short);
	~server();

	void listen(); // Constinuously listen for incoming client data.
};

#endif

// getRequest, doOperation, sendReply.
// Threads/Workers mechanism.

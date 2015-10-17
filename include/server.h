#ifndef SERVER_H
#define SERVER_H

#include "message.h"
#include "UDPsocket.h"

class server{
	server() = delete;
	server(const server&) = delete;
	server(server&&) = delete;
private:
	UDPsocket server_socket;

public:
	server(const char *, int);
	~server();

	void listen();
};

#endif
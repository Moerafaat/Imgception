#ifndef SERVER_H
#define SERVER_H

#include "message.h"
#include "socket.h"

class server {
	server() = delete;
	server(const server&) = delete;
	server(server&&) = delete;
private:
	socket *server_socket;

public:
	server(char *, int);
	~server();

	void listen();
};

#endif
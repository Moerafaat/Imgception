#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
#include "socket.h"

class client {
	client() = delete;
	client(const client&) = delete;
	client(client&&) = delete;
private:
	socket *client_socket;

public:
	client(char *, int);
	~client();

	message *execute(const message *) const;
};

#endif
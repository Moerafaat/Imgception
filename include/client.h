#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
#include "UDPsocket.h"

class client{
	client() = delete;
	client(const client&) = delete;
	client(client&&) = delete;
private:
	UDPsocket client_socket;

public:
	client(const char *, int);
	~client();

	message *execute(const message *);
};

#endif
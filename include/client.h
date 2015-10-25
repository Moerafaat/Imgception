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
	unsigned int server_ip;
	unsigned short server_port;
	unsigned int my_key;
	unsigned int nTries;
public:
	client(const char *, short);
	~client();

	int connect();
	void disconnect();
	void setTries(const unsigned int);
	
	message *execute(const message *); // Send an asynchronous message to the server.
 };

#endif

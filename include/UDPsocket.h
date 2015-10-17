#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

class UDPsocket{
	UDPsocket() = delete;
	UDPsocket(const UDPsocket&) = delete;
	UDPsocket(UDPsocket&&) = delete;
private:
	static const int maxBytes = 64*1024;
	int sock;
	socklen_t sin_size;
	sockaddr_in my_addr;
	sockaddr_in peer_addr;
	char buffer[maxBytes];
public:
	UDPsocket(const short);
	UDPsocket(const char * const, const short);
	~UDPsocket();

	char *syncRead(int&, const int=0);
	char *asyncRead(int&);
	int syncWrite(const char *, const int, const int=0);
	int asyncWrite(const char *, const int);

	int getMyPort() const;
	int getPeerPort() const;
};

#endif
#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class UDPsocket{
	UDPsocket() = delete;
public:
	UDPsocket(const UDPsocket&) = default;
	UDPsocket(UDPsocket&&) = default;
private:
	static const int maxBytes = 64*1024;
	int sock;
	socklen_t sin_size;
	sockaddr_in my_addr;
	sockaddr_in peer_addr;
	char buffer[maxBytes];

public:
	static char *resolveHostName(const char * const);
	UDPsocket(const short);
	UDPsocket(const char * const, const short, bool = false);
	~UDPsocket();

	UDPsocket clone() const;

	char *syncRead(int&, const int=0);
	char *asyncRead(int&);
	int syncWrite(const char *, const int, const int=0);
	int asyncWrite(const char *, const int);

	short getMyPort() const;
	short getPeerPort() const;
};

#endif

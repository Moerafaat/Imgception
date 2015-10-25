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
	static unsigned int resolveHostName(const char * const);
	UDPsocket();
	UDPsocket(const short);
	UDPsocket(const unsigned int, const short);
	~UDPsocket();

	char *syncRead(int&, const int=0);
	int asyncWrite(const char *, const int);
	void setReadTimeout(const unsigned int);

	void bindPeer(const unsigned int, const unsigned short);
	void releasePeer(const unsigned int=0, const unsigned short=0);
	void shutdownSocket();


	unsigned short getMyPort() const;
	unsigned short getPeerPort() const;
	unsigned int getMyIP() const;
	unsigned int getPeerIP() const;
};

#endif

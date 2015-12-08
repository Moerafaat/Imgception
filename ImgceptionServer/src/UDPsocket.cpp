#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "UDPsocket.h"

unsigned int UDPsocket::resolveHostName(const char* const HostName){ // Resolving the host name to the IP.
    hostent *host = gethostbyname(HostName);
    if (host == nullptr) return inet_addr(HostName);
    return ntohl(*(unsigned int *)host->h_addr);
}

UDPsocket::UDPsocket(): sin_size(sizeof(sockaddr_in)), sock(socket(AF_INET, SOCK_DGRAM, 0)){ // Construct a socket with random.
	if(sock == -1) throw("Error Creating Socket");
    FD_ZERO(&socket_set);
    FD_SET(sock, &socket_set);

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = 0;
    my_addr.sin_addr.s_addr = INADDR_ANY; // Listen from any interface.
	bzero(&(my_addr.sin_zero), 8);

	peer_addr.sin_family = AF_INET;
	bzero(&(peer_addr.sin_zero), 8);

	if (bind(sock, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1) throw("Error Binding");
	if(getsockname(sock, (sockaddr *)&my_addr, &sin_size) == -1) throw("Error Getting Sockname"); // Get own IP and Port.
}
UDPsocket::UDPsocket(const short my_port): sin_size(sizeof(sockaddr_in)), sock(socket(AF_INET, SOCK_DGRAM, 0)){ // Constuct a socket with a specific port.
	if(sock == -1) throw("Error Creating Socket");
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(void *)1,sizeof(1));
    FD_ZERO(&socket_set);
    FD_SET(sock, &socket_set);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(my_port);
    my_addr.sin_addr.s_addr = INADDR_ANY; // Listen from any interface.
    bzero(&(my_addr.sin_zero), 8);

	peer_addr.sin_family = AF_INET;
	bzero(&(peer_addr.sin_zero), 8);

	if (bind(sock, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1) throw("Error Binding");
}
UDPsocket::UDPsocket(const unsigned int IP, const short pport): sin_size(sizeof(sockaddr_in)), sock(socket(AF_INET, SOCK_DGRAM, 0)){
	if(sock == -1) throw("Error Creating Socket");
    FD_ZERO(&socket_set);
    FD_SET(sock, &socket_set);

	memset(&my_addr.sin_addr.s_addr, 0, sin_size);

    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(pport);
	peer_addr.sin_addr.s_addr = htonl(IP);
	bzero(&(peer_addr.sin_zero), 8);

	if(bind(sock, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1) throw("Error Binding"); // Explicit binding for the local socket.
	if(getsockname(sock, (sockaddr *)&my_addr, &sin_size) == -1) throw("Error Getting Sockname"); // Get own IP and Port.
}
UDPsocket::~UDPsocket(){
	close(sock);
}

char *UDPsocket::syncRead(int& size_read, const int msTimeOut){
    if(msTimeOut != -1){
        timeval tv;
        tv.tv_sec = msTimeOut / 1000;
        tv.tv_usec = msTimeOut % 1000 * 1000;
        FD_ZERO(&socket_set);
        FD_SET(sock, &socket_set);
        switch (select(FD_SETSIZE, &socket_set, nullptr, nullptr, &tv)){
            case -1: throw("Error on blocking syncRead call");
            case  0:
                size_read = -1;
                return nullptr;
        }
    }
	size_read = recvfrom(sock, buffer, maxBytes, 0, (sockaddr *)&peer_addr, &sin_size);
	if(size_read <= 0) return nullptr;
	char *buf = new char[size_read];
	memcpy(buf, buffer, size_read);
	return buf;
}
int UDPsocket::asyncWrite(const char *msg, const int size){
	return sendto(sock, msg, size, 0, (sockaddr *)&peer_addr, sin_size);
}

void UDPsocket::setReadTimeout(const unsigned int timeout_ms){
	timeval tv;
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = timeout_ms % 1000 * 1000;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval));
}
void UDPsocket::bindPeer(const unsigned int IP, const unsigned short port){
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = htons(port);
	peer_addr.sin_addr.s_addr = htonl(IP);
	bzero(&(peer_addr.sin_zero), 8);

	connect(sock, (sockaddr *)&peer_addr, sin_size);
}
void UDPsocket::releasePeer(const unsigned int IP, const unsigned short port){
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = 0;
	peer_addr.sin_addr.s_addr = 0;
	bzero(&(peer_addr.sin_zero), 8);

	connect(sock, (sockaddr *)&peer_addr, sin_size);

	peer_addr.sin_port = htons(port);
	peer_addr.sin_addr.s_addr = htonl(IP);
}
void UDPsocket::shutdownSocket(){
	if(close(sock) == -1) throw("Unable to shutdown socket");
}

unsigned short UDPsocket::getMyPort() const{
	return ntohs(my_addr.sin_port);
}
unsigned short UDPsocket::getPeerPort() const{
	return ntohs(peer_addr.sin_port);
}
unsigned int UDPsocket::getMyIP() const{
	return ntohl(my_addr.sin_addr.s_addr);
}
unsigned int UDPsocket::getPeerIP() const{
	return ntohl(peer_addr.sin_addr.s_addr);
}

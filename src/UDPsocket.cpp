#include <iostream>
#include "UDPsocket.h"

using namespace std;

char *UDPsocket::resolveHostName(const char* const HostName){ // Resolving the host name to the IP.
	char* IP = new char[4 / sizeof(char)];
	hostent *host = gethostbyname(HostName);
	if (host == NULL) throw("Cannot Resolve Host");
	memcpy(IP, host->h_addr, 4);
}

UDPsocket::UDPsocket(const short my_port): sin_size(sizeof(sockaddr_in)), sock(socket(AF_INET, SOCK_DGRAM, 0)){ // Constuct server socket.
	if(sock == -1) throw("Error Creating Socket");

	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(void *)1,sizeof(1));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(my_port);
    my_addr.sin_addr.s_addr = INADDR_ANY; // Listen for any incoming IPs.
    bzero(&(my_addr.sin_zero), 8);
    if (bind(sock, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1) throw("Error Binding");
}

UDPsocket::UDPsocket(const char * const IP, const short pport, bool cleanIP): sin_size(sizeof(sockaddr_in)), sock(socket(AF_INET, SOCK_DGRAM, 0)){
	// Construct a new socket to communicate with the peer provided in the argument.
	if(sock == -1) throw("Error Creating Socket");

	memset((char *) &peer_addr, 0,sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(pport);

	memcpy((char *)&peer_addr.sin_addr.s_addr, IP, 4);
    memset(&(peer_addr.sin_zero), 0, 8);
    if(cleanIP) delete [] IP;
}

UDPsocket::~UDPsocket(){
	close(sock);
}

UDPsocket UDPsocket::clone() const{ // Clone the socket (Used to reserve a different port for each incoming client).
	return UDPsocket((char *)&peer_addr.sin_addr.s_addr, peer_addr.sin_port);
}

char *UDPsocket::syncRead(int&, const int){
	int size_read = recvfrom(sock, buffer, maxBytes, 0, (sockaddr *)&peer_addr, &sin_size);

	char *buf = new char[size_read];
	memcpy(buf, buffer, size_read);
	return buf;
}

char *UDPsocket::asyncRead(int&){ // Not yet implemented.
	return 0;
}

int UDPsocket::syncWrite(const char *, const int, const int){ // Not yet implemented.
	return 0;
}

int UDPsocket::asyncWrite(const char * msg, const int size){
	return sendto(sock, msg, size, 0, (sockaddr *)&peer_addr, sin_size);
}

short UDPsocket::getMyPort() const{
	return my_addr.sin_port;
}

short UDPsocket::getPeerPort() const{
	return peer_addr.sin_port;
}

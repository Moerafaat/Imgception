#include "UDPsocket.h"

UDPsocket::UDPsocket(const short my_port): sin_size(sizeof(sockaddr_in)), sock(socket(AF_INET, SOCK_DGRAM, 0)){
	if(sock == -1) throw("Error Creating Socket");

	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(void *)1,sizeof(1));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(my_port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);
    if (bind(sock, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1) throw("Error Binding");
}

UDPsocket::UDPsocket(const char * const paddr, const short pport): sin_size(sizeof(sockaddr_in)), sock(socket(AF_INET, SOCK_DGRAM, 0)){
	if(sock == -1) throw("Error Creating Socket");
	
	memset((char *) &peer_addr, 0,sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(pport);
    hostent *server = gethostbyname(paddr);
	if (server == NULL) throw("Cannot Reach Server");
	memcpy((char *)&peer_addr.sin_addr.s_addr,(char *)server->h_addr, server->h_length);
    memset(&(peer_addr.sin_zero), 0, 8);
}

UDPsocket::~UDPsocket(){
	close(sock);
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

int UDPsocket::getMyPort() const{
	return my_addr.sin_port;
}

int UDPsocket::getPeerPort() const{
	return peer_addr.sin_port;
}
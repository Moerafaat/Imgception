#ifndef SOCKET_H
#define SOCKET_H

class socket {
	socket() = delete;
	socket(const socket&) = delete;
	socket(socket&&) = delete;
private:
	int sock;
	sockaddr_in my_addr;
	sockaddr_in peer_addr;
	int my_port;
	int peer_port;
	char *my_address;
	char *peer_address;

public:
	socket(const int);
	socket(const char const *, const int);
	~socket();

	int syncRead(char *, const size_t, const int);
	int asyncRead(char *, const size_t);
	int syncWrite(const char *, const size_t, const int);
	int asyncWrite(const char *, const size_t);

	int getMyPort() const;
	int getPeerPort() const;
};

#endif
#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "message.h"
#include "UDPsocket.h"
#include "worker.h"
using namespace std;

class server{
	server() = delete;
	server(const server&) = delete;
	server(server&&) = delete;
private:
	static const int MAX_WORKERS = 100;
	UDPsocket server_socket;
	worker* workers[MAX_WORKERS];
	vector<worker *> freeWorkers;	//Index or ptr
	pthread_mutex_t workersLockMutex;
	bool EXIT_FLAG;
public:
	server(const char *, short);
	~server();

	void listen(); // Constinuously listen for incoming client data.
	worker* lockWorker();
	void unlockWorker(worker *);
	void cleanExit();
	bool exitStatus() const;
};

#endif

// getRequest, doOperation, sendReply.
// Threads/Workers mechanism.

#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>
#include "message.h"
#include "UDPsocket.h"

class server;

class worker{
	worker() = delete;
	worker(const worker&) = delete;
	worker(worker&&) = delete;
private:
	static void *process (void *); // Process to be spawned in a thread.
	UDPsocket socket;
	pthread_t thread_id;
	pthread_mutex_t available;
	unsigned int clientKey;
	server *Server;

public:
	worker(server *);
	~worker();

	void deploy(const unsigned int, const unsigned int, const unsigned short); //API to deploy thread with IP:Port
};

#endif

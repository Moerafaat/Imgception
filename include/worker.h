#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>
#include "message.h"
#include "UDPsocket.h"

class worker{
	worker() = delete;
	worker(const worker&) = delete;
	worker(worker&&) = delete;
private:
	static void *process (void *); // Process to be spawned in a thread.
	message msg;
	UDPsocket socket;
	pthread_t thread_id;
	char ip_port[6];

public:
	worker(message *, UDPsocket);
	~worker();

	void deploy(); // API that is used to construct the pthread.

	const char *getIPandPort() const; // Return the IP and Port to send to client.
};

#endif

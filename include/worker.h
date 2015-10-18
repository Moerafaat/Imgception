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
	static void *process (void *);
	message msg;
	UDPsocket socket;
	pthread_t thread_id;

public:
	worker(message *, UDPsocket);
	~worker();
	void deploy();
};

#endif

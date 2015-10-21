#include <iostream>
#include "worker.h"
#include "server.h"

using namespace std;

void *worker::process(void * W){
	worker *Worker = (worker *)W;
	if(pthread_mutex_lock(&Worker->available)) throw("Unable to lock mutex");
	do{
		cout << "Hello from worker to " << Worker->socket.getPeerPort() << " with key: " << Worker->clientKey << endl;
		Worker->clientKey = -1;

		Worker->Server->unlockWorker(Worker);
		// Do not insert any instructions here to avoid undefined states in 'socket' and 'clientKey'.
		if(pthread_mutex_lock(&Worker->available)) throw("Unable to lock mutex");

	}while(!Worker->Server->exitStatus());
}

worker::worker(server* ser) : Server(ser), clientKey(-1){
	if(pthread_create(&thread_id, nullptr, process, this) < 0) throw("Error Creating Thread");
	if(pthread_mutex_init(&available, nullptr))	throw("Unable to initialize mutex"); //Initialize and unlock
	if(pthread_mutex_lock(&available)) throw("Unable to lock mutex");
}

worker::~worker(){
	if(pthread_mutex_unlock(&available)) throw("Unable to unlock mutex");
	if(pthread_join(thread_id, nullptr)) throw("Unable to join thread");
	if(pthread_mutex_destroy(&available)) throw("Unable to destroy mutex");
}

void worker::deploy(const unsigned int cKey, const unsigned int ip, const unsigned short port){
	socket.bindPeer(ip, port);
	clientKey = cKey;
	if(pthread_mutex_unlock(&available)) throw("Unable to unlock mutex");
}

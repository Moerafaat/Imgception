#include <iostream>
#include "worker.h"
#include "server.h"

using namespace std;

void *worker::process(void * W){
	try{
	worker *Worker = (worker *)W;
	char *buf;
	if(pthread_mutex_lock(&Worker->available)) throw("Unable to lock mutex");
	while(!Worker->Server->exitStatus()){
		cout << hex << W << " Hello from worker to " << Worker->socket.getPeerPort() << " with key: " << Worker->clientKey << endl;
		unsigned int msg = htonl(Worker->clientKey);
		int size = Worker->socket.asyncWrite((char *)&msg, sizeof(Worker->clientKey));

		while(true){
			buf = Worker->socket.syncRead(size);
			cout << size - 1 << " " << buf + 1 << endl;
			if(buf[1] == 'q'){
				Worker->Server->cleanExit(); // Calls server exit function.
				break;
			}
			delete [] buf;
		}
		delete [] buf;
		Worker->clientKey = -1;

		Worker->socket.bindPeer(Worker->socket.getMyIP(), Worker->socket.getMyPort());	//Check if there is a better way.
		Worker->Server->unlockWorker(Worker);
		// Do not insert any instructions here to avoid undefined states in 'socket' and 'clientKey'.
		if(pthread_mutex_lock(&Worker->available)) throw("Unable to lock mutex");
	}while(!Worker->Server->exitStatus());
	if(pthread_mutex_unlock(&Worker->available)) throw("Unable to unlock mutex");	//No Idea where is it locked
	}catch(const char* str){
		cout << str << endl;
	}
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

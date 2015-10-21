#include <iostream>
#include "server.h"

using namespace std;

server::server(const char *, short my_port): server_socket(my_port), EXIT_FLAG(false){ // Construct server socket.
	for(int i = 0; i < MAX_WORKERS; i++) freeWorkers.push_back(workers[i] = new worker(this));
	if(pthread_mutex_init(&workersLockMutex, nullptr))	throw("Unable to initialize mutex"); //Initialize and unlock
}

server::~server(){
	for(int i = 0; i < MAX_WORKERS; i++) delete workers[i];
	if(pthread_mutex_destroy(&workersLockMutex)) throw("Unable to destroy mutex");
}

void server::listen(){
	char *buffer;
	int size;
	while(!EXIT_FLAG){ // Continuously listening for incoming client data.
		buffer = server_socket.syncRead(size);
		lockWorker()->deploy(ntohs(*(unsigned short*)buffer), server_socket.getPeerIP(), server_socket.getPeerPort());
		delete [] buffer;
	}
}

worker* server::lockWorker(){
	if(pthread_mutex_lock(&workersLockMutex)) throw("Unable to lock mutex");	//we need try lock here
	while(freeWorkers.size() == 0);	//busy wait for now
	worker *returnWorker = freeWorkers.back();
	freeWorkers.pop_back();
	if(pthread_mutex_unlock(&workersLockMutex)) throw("Unable to unlock mutex");
	return returnWorker;
}

void server::unlockWorker(worker* Worker){
	if(pthread_mutex_lock(&workersLockMutex)) throw("Unable to lock mutex");
	freeWorkers.push_back(Worker);
	if(pthread_mutex_unlock(&workersLockMutex)) throw("Unable to unlock mutex");
}

void server::cleanExit(){
	EXIT_FLAG = true;
}

bool server::exitStatus() const{
	return EXIT_FLAG;
}

// Threads/Workers mechanism.
// Workers are still not yet deleted (Intentional until it is discussed in class).

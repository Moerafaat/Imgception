#include <cstring>
#include <ctime>
#include <iostream>
#include "Views.h"

//ServerMessage
unsigned int ServerMessage::nextID = time(0);
ServerMessage::ServerMessage(const unsigned char v) : msgID(nextID++), msgVector(v), payload(nullptr), size(0){

}
ServerMessage::ServerMessage(const unsigned int ID, const unsigned char v) : msgID(ID), msgVector(v), payload(nullptr), size(0){

}
ServerMessage::ServerMessage(ServerMessage&& cpy) : msgID(cpy.msgID), msgVector(cpy.msgVector), payload(cpy.payload), size(cpy.size){
    cpy.payload = nullptr;
}
ServerMessage::~ServerMessage(){
    if(payload != nullptr) delete [] payload;
}

ServerMessage& ServerMessage::operator=(const ServerMessage& cpy){
    if(payload != nullptr) delete [] payload;
    msgID = cpy.msgID;
    msgVector = cpy.msgVector;
    size = cpy.size;
    if(cpy.payload != nullptr){
        payload = new char[size];
        memcpy(payload, cpy.payload, size);
    }
    else payload = nullptr;
    return *this;
}
ServerMessage& ServerMessage::operator=(ServerMessage&& cpy){
    if(payload != nullptr) delete [] payload;
    msgID = cpy.msgID;
    msgVector = cpy.msgVector;
    size = cpy.size;
    payload = cpy.payload; cpy.payload = nullptr;
    return *this;
}

char* ServerMessage::serialize(int& sz) const{
    char* ret = new char[sz = 4 + 1 + 4 + size];
    *(unsigned int*)ret = HostToNetwork(msgID);
    ret[4] = msgVector;
    *(unsigned int*)(ret+5) = HostToNetwork(size);
    if(size != 0) memcpy(ret + 9, payload, size);
    return ret;
}
bool ServerMessage::deserialize(const char * const buf, const unsigned int sz){
    if(sz < 9) return false;
    msgID = NetworkToHost(*(unsigned int*)buf);
    msgVector = buf[4];
    size = NetworkToHost(*(unsigned int*)(buf + 5));
    payload = nullptr;
    if(size + 9 != sz) return false;
    if(size != 0) (payload = new char[size], buf+9, size);
    return true;
}

unsigned int ServerMessage::getID() const{
    return msgID;
}
unsigned char ServerMessage::getVector() const{
    return msgVector;
}
const char *ServerMessage::getPayload() const{
    return payload;
}
const unsigned int ServerMessage::getPayloadSize() const{
    return size;
}
void ServerMessage::setPayload(const char * const buffer, const unsigned int sz){
    if(payload != nullptr) delete [] payload;
    size = sz;
    if(size == 0) payload = nullptr;
    else{
        payload = new char[size];
        memcpy(payload, buffer, size);
    }
}


//ServerView
ServerView::ServerView(unsigned short my_port): server_socket(my_port), EXIT_FLAG(false){ // Construct server socket.
	for(int i = 0; i < MAX_WORKERS; i++) freeWorkers.push_back(workers[i] = new WorkerView(this));
	if(pthread_mutex_init(&workersLockMutex, nullptr))	throw("Unable to initialize mutex"); //Initialize and unlock
}
ServerView::~ServerView(){
	for(int i = 0; i < MAX_WORKERS; i++) delete workers[i];
	if(pthread_mutex_destroy(&workersLockMutex)) throw("Unable to destroy mutex");
}

WorkerView *ServerView::lockWorker(){
	if(pthread_mutex_lock(&workersLockMutex)) throw("Unable to lock mutex");	//we need try lock here when managing Queuing (i.e. running out of workers)
	WorkerView *returnWorker = freeWorkers.back();
	freeWorkers.pop_back();
	if(pthread_mutex_unlock(&workersLockMutex)) throw("Unable to unlock mutex");
	return returnWorker;
}
void ServerView::unlockWorker(WorkerView *Worker){
	if(pthread_mutex_lock(&workersLockMutex)) throw("Unable to lock mutex");
	freeWorkers.push_back(Worker);
	if(pthread_mutex_unlock(&workersLockMutex)) throw("Unable to unlock mutex");
}

void ServerView::setCallbackFunc(const unsigned char vector, void(*func)(WorkerView&, const ServerMessage&)){
    callbackFunc[vector] = func;
}
ServerMessage ServerView::listen(unsigned int& fromIP, unsigned short& fromPort){
	char *buffer;
    int size;   //Should checking redundency be done here? OR in the stub? I think the stub
    buffer = server_socket.syncRead(size);
    if(buffer == nullptr) return ServerMessage(0xFF);       //Error Message
    fromIP = server_socket.getPeerIP();
    fromPort = server_socket.getPeerPort();
    ServerMessage ret;
    ret.deserialize(buffer, size);
    delete [] buffer;
    return ret;
}
void ServerView::deployWorker(const unsigned int peerIP, const unsigned short peerPort, const ServerMessage& msg){
	if(freeWorkers.size()) lockWorker()->deploy(peerIP, peerPort, msg);
}
void ServerView::cleanExit(){   //Should be called from the same thread as listening. Until a future solution if found
	if(!EXIT_FLAG){
        EXIT_FLAG = true;
        server_socket.bindPeer(server_socket.getMyIP(), server_socket.getMyPort());    //To avoid filling the buffer. Maybe not needed anymore?
    }
}
bool ServerView::exitStatus() const{
	return EXIT_FLAG;
}
bool ServerView::isWorkerFree() const{
    return freeWorkers.size();
}

//WorkerView
void *WorkerView::process(void * W){
	WorkerView *Worker = (WorkerView *)W;
    char *buf;
    int size;
    if(pthread_mutex_lock(&Worker->available)) throw("Unable to lock mutex");
	while(!Worker->Server->exitStatus()){
        cout << "LOG::Worker deployed for " << Worker->worker_socket.getPeerIP() << "::" << Worker->worker_socket.getPeerPort() << "." << endl;

        buf = ServerMessage(Worker->initMsg.getID()+1, 0xFD).serialize(size);
        Worker->worker_socket.asyncWrite(buf, size);
        delete [] buf;

        //Call back functions based on msgVector
        if(Worker->Server->callbackFunc[Worker->initMsg.getVector()] != nullptr) Worker->Server->callbackFunc[Worker->initMsg.getVector()](*Worker, Worker->initMsg);
        else if(Worker->Server->callbackFunc[0xFF] != nullptr) Worker->Server->callbackFunc[0xFF](*Worker, Worker->initMsg);
        else cout << "LOG::Error. Unrecognized message vector." << endl;

        //Disconnect  We may do some handshaking to notify the client or something but I don't think we need it

        cout << "LOG::Worker exiting!" << endl;
        Worker->worker_socket.bindPeer(Worker->worker_socket.getMyIP(), Worker->worker_socket.getMyPort());	//Check if there is a better way. It is needed to avoid filling the buffer while not active.
        Worker->initMsg = ServerMessage();
		Worker->Server->unlockWorker(Worker);
		//Do not access/modify Worker here avoid undefined states.
		if(pthread_mutex_lock(&Worker->available)) throw("Unable to lock mutex");
	}
	if(pthread_mutex_unlock(&Worker->available)) throw("Unable to unlock mutex");
}

WorkerView::WorkerView(ServerView* srvr) : Server(srvr), initMsg(){
    worker_socket.bindPeer(worker_socket.getMyIP(), worker_socket.getMyPort());	//Check if there is a better way. It is needed to avoid filling the buffer while not active.
    if(pthread_mutex_init(&available, nullptr))	throw("Unable to initialize mutex"); //Initialize and unlock
	if(pthread_mutex_lock(&available)) throw("Unable to lock mutex");
    if(pthread_create(&thread_id, nullptr, process, this) < 0) throw("Error Creating Thread");
}

WorkerView::~WorkerView(){
	if(pthread_mutex_unlock(&available)) throw("Unable to unlock mutex");
	if(pthread_join(thread_id, nullptr)) throw("Unable to join thread");
	if(pthread_mutex_destroy(&available)) throw("Unable to destroy mutex");
}

void WorkerView::deploy(const unsigned int ip, const unsigned short port, const ServerMessage& msg){
	worker_socket.bindPeer(ip, port);
	initMsg = msg;
	if(pthread_mutex_unlock(&available)) throw("Unable to unlock mutex");
}
int WorkerView::sendObject(const transmittable * const obj){
    //No fragmentation yet
    char *buf;
    unsigned int size;
    buf = obj->serialize(size);
    if(worker_socket.asyncWrite(buf, size) < 0){
        delete [] buf;
        return -1;
    }
    return 0;
}
int WorkerView::recieveObject(transmittable * const obj, const int timeout){
    //No fragmentation yet
    char *buf;
    int size;
    do{
        if((buf = worker_socket.syncRead(size, timeout)) == nullptr) return -1;
        if(!obj->deserialize(buf, size)){
            delete [] buf;
            continue;
        }
        break;
    }while(true);
    delete [] buf;
    return 0;
}

//ClientView
//client_socket(ntohl(inet_addr(hostname)), peer_port)
ClientView::ClientView(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port){
	//Resovles the host first, then constructs the client socket.
	server_ip = client_socket.getPeerIP();
	server_port = peer_port;
}

int ClientView::connect(const ServerMessage& msg, const int timeout){
    int size;
    char* buf;
    int serializedMSGSize;
    char *serializedMSG = msg.serialize(serializedMSGSize);

    if(client_socket.asyncWrite(serializedMSG, serializedMSGSize) < 0) return -1;
    delete [] serializedMSG;

    do{
        if(timeout == 0) return 0;
        if((buf = client_socket.syncRead(size, timeout)) == nullptr){
            client_socket.releasePeer(server_ip, server_port);
            cout << "LOG::Unable to read or timeout" << endl;
            return -1;
        }
        ServerMessage reply;
        if(!reply.deserialize(buf, size)){
            cout << "LOG::Unable to deserialize" << endl;
            delete [] buf;
            continue;
        }
        delete [] buf;
        if(reply.getID() != msg.getID() + 1 || reply.getVector() != 0xFD && reply.getPayloadSize() != 0){
            cout << "LOG::Invalid packet" << endl;
            continue;
        }
        break;
    }while(true);
    client_socket.bindPeer(client_socket.getPeerIP(), client_socket.getPeerPort());
    cout << "LOG::Sucess" << endl;

    return 0;
}
void ClientView::disconnect(){
	client_socket.releasePeer(server_ip, server_port);
}
int ClientView::sendObject(const transmittable * const obj){
    //No fragmentation yet
    char *buf;
    unsigned int size;
    buf = obj->serialize(size);
    if(client_socket.asyncWrite(buf, size) < 0){
        delete [] buf;
        return -1;
    }
    return 0;
}
int ClientView::recieveObject(transmittable * const obj, const int timeout){
    //No fragmentation yet
    char *buf;
    int size;
    do{
        if((buf = client_socket.syncRead(size, timeout)) == nullptr) return -1;
        if(!obj->deserialize(buf, size)){
            delete [] buf;
            continue;
        }
        break;
    }while(true);
    delete [] buf;
    return 0;
}

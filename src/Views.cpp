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
ServerMessage::ServerMessage(const char* buf) : msgID(NetworkToHost(*(unsigned int*)buf)), msgVector(buf[4]), size(NetworkToHost(*(unsigned int*)(buf + 5))), payload(nullptr){
    //Need to check buf sizes to avoid crashes
    if(size != 0)
        memcpy(payload = new char[size], buf+9, size);
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

ServerMessage ServerView::listen(unsigned int& fromIP, unsigned short& fromPort){
	char *buffer;
    int size;   //Should checking redundency be done here? OR in the stub? I think the stub
    buffer = server_socket.syncRead(size);
    if(buffer == nullptr) return ServerMessage(0xFF);       //Error Message
    fromIP = server_socket.getPeerIP();
    fromPort = server_socket.getPeerPort();
    ServerMessage ret(buffer);
    delete [] buffer;
    return ret;
}
void ServerView::deployWorker(const unsigned int peerIP, const unsigned short peerPort, const ServerMessage& msg){
	lockWorker()->deploy(peerIP, peerPort, msg);   //Should add Queuing
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

//WorkerView
void *WorkerView::process(void * W){
	WorkerView *Worker = (WorkerView *)W;
    char *buf;
    int size;
    if(pthread_mutex_lock(&Worker->available)) throw("Unable to lock mutex");
	while(!Worker->Server->exitStatus()){
        cout << "LOG::Worker deployed for " << Worker->worker_socket.getPeerIP() << "::" << Worker->worker_socket.getPeerPort() << "." << endl;

        buf = ServerMessage(Worker->initMsg.getID()+1, 0x00).serialize(size);

        Worker->worker_socket.asyncWrite(buf, size);
        delete [] buf;

        //Call back functions based on msgVector
        //...
        switch(Worker->initMsg.getVector()){
        case 0x01:  //Sending Message
        {   //some callback that does the following
            //Reading/Sending message should be encapsulated in a Worer::SomeFuntion(transmittable*);
            //Sending/Reading Message functions should be done in a way to garentee dilievery exactly once or faliure reported.
            //Thus, here UDP::Read/UDP::Write should be replaced with those function and here will assume that they will always succeed or if faild I will not try again (Other wise the stub shold handel faliures if he wants to try again).

            if((buf = Worker->worker_socket.syncRead(size, 10000)) == nullptr){
                cout << "LOG::Error while proccessing Get Vector. Response timedout." << endl;
                break;
            }
            if(size <= 0){
                cout << "LOG::Error while proccessing Get Vector." << endl;
                delete [] buf;
                break;
            }
            //Sould be done with some transmittable* and exported to the stub and deserialization.
            //For now I am using ServerMessage.
            ServerMessage msg(buf); delete [] buf;
            if(msg.getVector() != 0x02 || msg.getPayload() == nullptr){
                cout << "Error... Invalid packet." << endl;
                break;
            }

            //Sending acknowledgment
            buf = ServerMessage(msg.getID()+1, 0xFE).serialize(size);
            if(Worker->worker_socket.asyncWrite(buf, size) < 0){
                cout << "Unable to send acknowledgment." << endl;
                delete [] buf;
                break;
            }
            cout << msg.getPayload() << endl;
            delete [] buf;
        }
        break;
        default:
            cout << "LOG::Message Vector is not recognized." << endl;
        }
        //End if call back

        //Disconnect  We may do some handshaking to notify the client or something but I don't think we need it

        cout << "LOG::Worker exiting!" << endl;
        Worker->initMsg = ServerMessage();
        Worker->worker_socket.bindPeer(Worker->worker_socket.getMyIP(), Worker->worker_socket.getMyPort());	//Check if there is a better way. It is needed to avoid filling the buffer while not active.
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



//ClientView
//client_socket(ntohl(inet_addr(hostname)), peer_port)
ClientView::ClientView(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port){
	//Resovles the host first, then constructs the client socket.
	server_ip = client_socket.getPeerIP();
	server_port = peer_port;
}

bool ClientView::connect(const ServerMessage& msg, const unsigned int nTries){
    if(nTries == 0) return false;
	int size;
    char* buf;
    int serializedMSGSize;
    char *serializedMSG = msg.serialize(serializedMSGSize);
	for(int i = 0; i < nTries; i++){
        cout << "Trial " << i+1 << "..." << endl;
        int t;
        if(client_socket.asyncWrite(serializedMSG, serializedMSGSize) < 0){
            cout << "Trial " << i+1 << " Failed. Unable to send packet." << endl;
            continue; //Don't remember the error codes need to check (Didn't have internet access when coding this)
        }
        if((buf = client_socket.syncRead(size, 5000)) == nullptr){
            cout << "Trial " << i+1 << " Failed. Response timeout." << endl;
            continue;
        }
        if(size < 0){
            cout << "Trial " << i+1 << " Failed. Error occured while reading." << endl;
            client_socket.releasePeer(server_ip, server_port);
            delete [] buf;
            continue;
        }

        ServerMessage reply(buf); delete [] buf;
		if(msg.getID() + 1 != reply.getID() || reply.getVector() != 0x00 && reply.getPayloadSize() != 0){
            cout << "Trial " << i+1 << " Failed. Invalid packet." << endl;
            client_socket.releasePeer(server_ip, server_port);
            continue;
        }
        client_socket.bindPeer(client_socket.getPeerIP(), client_socket.getPeerPort());
        cout << "Trial " << i+1 << " Successful." << endl;
        //I think we might 2 more handshakes (send to worker, recieve from worker) or at least 1 more (send to worker ack) It is better so that the worker knows if the client is reachable.
        //On a second thought, I think it might be harder than it seems. How should the client check if there is duplicates?
        delete [] serializedMSG;
        return true;
	}
    delete [] serializedMSG;
    return false;
}
void ClientView::disconnect(){
	client_socket.releasePeer(server_ip, server_port);
}
bool ClientView::send(const ServerMessage& msg){
    int size;
    char *buf = msg.serialize(size);
    //Assuming no need for fragmentation
    if(client_socket.asyncWrite(buf, size) < 0){
        cout << "Unable to send packet." << endl;
        delete [] buf;
        return false; //Don't remember the error codes need to check (Didn't have internet access when coding this)
    }
    delete [] buf;
    return true;
}
ServerMessage ClientView::recieve(){
    char *buf;
    int size;
    if((buf = client_socket.syncRead(size, 5000)) == nullptr){
        cout << "LOG::Error. Timeout on Recieve." << endl;
        return ServerMessage(0xFF);
    }
    if(size < 0){
        cout << "LOG::Error recievning message." << endl;
        client_socket.releasePeer(server_ip, server_port);
        delete [] buf;
        return ServerMessage(0xFF);
    }
    ServerMessage msg(buf);
    delete [] buf;
    return msg;
}
/*headerMessage *ClientView::execute(const headerMessage *msg){
	string tstr;
	tstr.push_back(1);
	tstr += *reinterpret_cast<const string*>(msg);
	int size = client_socket.asyncWrite(tstr.c_str(), tstr.size() + 1);

	return nullptr;
}*/

// Send Reply.
// Port to Worker.
// Handle spamming.

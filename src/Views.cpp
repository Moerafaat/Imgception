#include <cstring>
#include <ctime>
#include <iostream> //Should remove this after done with testing
#include "Views.h"
#include "transmittable.h"
using namespace std;

class FragmentDrive{
    FragmentDrive(const FragmentDrive&) = delete;
    FragmentDrive(FragmentDrive&&) = delete;
public:
    static const unsigned int MaxTransmittableSize = 1 << 30;
    static const unsigned short MaxFragmentSize = 2048;  //Probably much higher
    static const unsigned int MaxFragmentCount = MaxTransmittableSize / MaxFragmentSize;
    static const unsigned short BatchCount = 10;  //Not sure what is a suitable number.
private:
    static unsigned int nextID;
public:
    unsigned int MessageID;
    unsigned int FragmentDriveID;
    unsigned int MessageSize;
    unsigned int FragmentCount;
    unsigned int NextFragmentID;
public:
    FragmentDrive() = default;
    FragmentDrive(const unsigned int msgSize): MessageID(nextID++), FragmentDriveID(0), MessageSize(msgSize), FragmentCount((msgSize + MaxFragmentSize - 1)/ MaxFragmentSize), NextFragmentID(-1){
        if(msgSize > MaxTransmittableSize) throw("ERROR::Transmittable is too big.");
    }
    ~FragmentDrive() = default;

    FragmentDrive& operator=(const FragmentDrive& cpy){
        MessageID = cpy.MessageID;
        FragmentDriveID = cpy.FragmentDriveID;
        MessageSize = cpy.MessageSize;
        FragmentCount = cpy.FragmentCount;
        NextFragmentID = cpy.NextFragmentID;
        return *this;
    }
    bool operator>(const FragmentDrive& val){
        return MessageID == val.MessageID && FragmentDriveID > val.FragmentDriveID && MessageSize == val.MessageSize && FragmentCount == val.FragmentCount && NextFragmentID > val.NextFragmentID;
    }

    char *serialize(unsigned int& size){
        size = 4 * 5;
        char *buf = new char[size];
        *(unsigned int*)(buf +  0) = HostToNetwork(MessageID);
        *(unsigned int*)(buf +  4) = HostToNetwork(FragmentDriveID);
        *(unsigned int*)(buf +  8) = HostToNetwork(MessageSize);
        *(unsigned int*)(buf + 12) = HostToNetwork(FragmentCount);
        *(unsigned int*)(buf + 16) = HostToNetwork(NextFragmentID);
        return buf;
    }
    bool deserialize(const char * const buf, const unsigned int size){
        if(size != 4 * 5) return false;
        MessageID       = NetworkToHost(*(unsigned int*)(buf +  0));
        FragmentDriveID = NetworkToHost(*(unsigned int*)(buf +  4));
        MessageSize     = NetworkToHost(*(unsigned int*)(buf +  8));
        FragmentCount   = NetworkToHost(*(unsigned int*)(buf + 12));
        NextFragmentID  = NetworkToHost(*(unsigned int*)(buf + 16));
        return true;
    }
};
unsigned int FragmentDrive::nextID = time(0);

class Fragment{
    Fragment(const Fragment&) = delete;
    Fragment(Fragment&&) = delete;

public:
    unsigned int MessageID;
    unsigned int FragmentID;
    unsigned int FragmentSize;
    char Payload[FragmentDrive::MaxFragmentSize];
public:
    Fragment() = default;
    ~Fragment() = default;
    char *serialize(unsigned int& size){
        size = 4 * 3 + FragmentSize;
        char *buf = new char[size];
        *(unsigned int*)(buf +  0) = HostToNetwork(MessageID);
        *(unsigned int*)(buf +  4) = HostToNetwork(FragmentID);
        *(unsigned int*)(buf +  8) = HostToNetwork(FragmentSize);
        memcpy(buf + 12, Payload, FragmentSize);
        return buf;
    }
    bool deserialize(const char * const buf, const unsigned int size){
        if(size < 12) return false;
        MessageID      = NetworkToHost(*(unsigned int*)(buf +  0));
        FragmentID     = NetworkToHost(*(unsigned int*)(buf +  4));
        FragmentSize   = NetworkToHost(*(unsigned int*)(buf +  8));
        if(size != 12 + FragmentSize) return false;
        memcpy(Payload, buf + 12, FragmentSize);
        return true;
    }
};

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

char* ServerMessage::serialize(unsigned int& sz) const{
    char* ret = new char[sz = 4 + 1 + 4 + size];
    *(unsigned int*)ret = HostToNetwork(msgID);
    ret[4] = msgVector;
    *(unsigned int*)(ret+5) = HostToNetwork(size);
    if(size != 0) memcpy(ret + 9, payload, size);
    return ret;
}
bool ServerMessage::deserialize(const char * const buf, const int sz){
    if(sz < 9) return false;
    msgID = NetworkToHost(*(unsigned int*)buf);
    msgVector = buf[4];
    size = NetworkToHost(*(unsigned int*)(buf + 5));
    payload = nullptr;
    if(size + 9 != sz) return false;
    if(size > FragmentDrive::MaxFragmentSize) return false;
    if(size != 0) memcpy(payload = new char[size], buf+9, size);
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
    if(sz > FragmentDrive::MaxFragmentSize) throw("Payload is too large to contain in ServerMessage.");
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
void ServerView::cleanExit(){ //Should be called from the same thread as listening. Until a future solution if found
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

//WorkerView
void *WorkerView::process(void * W){
	WorkerView *Worker = (WorkerView *)W;
    char *buf;
    unsigned int size;
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

WorkerView::WorkerView(ServerView *srvr) : Server(srvr), initMsg(){
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
bool WorkerView::sendObject(const Transmittable * const obj, const unsigned int nTries){
    char *buf;
    unsigned int size;
    buf = obj->serialize(size);
    if(size == 0){
        delete [] buf;
        return false;
    }
    FragmentDrive drive(size);
    FragmentDrive incDrive;
    vector<Fragment> Frag(drive.FragmentCount);
    for(int i = 0; i < drive.FragmentCount; i++){
        Frag[i].MessageID = drive.MessageID;
        Frag[i].FragmentID = i;
        Frag[i].FragmentSize = (Frag[i].FragmentID + 1 == drive.FragmentCount && size ) ? (size - 1) % FragmentDrive::MaxFragmentSize + 1: FragmentDrive::MaxFragmentSize;
        memcpy(Frag[i].Payload, buf + Frag[i].FragmentID * FragmentDrive::MaxFragmentSize, Frag[i].FragmentSize);
    }
    delete [] buf;
    int failCount = 0;
    buf = drive.serialize(size);
    while(worker_socket.asyncWrite(buf, size) < 0 && failCount < nTries) failCount++;
    delete [] buf;
    for(drive.NextFragmentID = 0; drive.NextFragmentID < drive.FragmentCount; ){
        for(int i = 0; i < FragmentDrive::BatchCount && drive.NextFragmentID + i < drive.FragmentCount; i++){
            buf = Frag[drive.NextFragmentID + i].serialize(size);
            failCount += worker_socket.asyncWrite(buf, size) < 0;
            delete [] buf;
        }
        while(failCount < nTries*FragmentDrive::BatchCount && (buf = worker_socket.syncRead((int&)size, 500)) != nullptr){
            if(incDrive.deserialize(buf, size) && incDrive > drive){
                drive = incDrive;
                failCount -= failCount > 0;
                delete [] buf;
                break;
            }
            failCount++;
            delete [] buf;
        }
        failCount += buf == nullptr;
        if(failCount >= nTries * FragmentDrive::BatchCount) return false;
    }
    return true;
}
bool WorkerView::recieveObject(Transmittable * const obj, const unsigned int nTries){
    char *buf;
    unsigned int size;

    FragmentDrive drive;
    int failCount = 0;
    while(failCount < nTries)
        if((buf = worker_socket.syncRead((int&)size, 1000)) == nullptr || !drive.deserialize(buf, size)){
            if(buf != nullptr) delete [] buf;
            failCount++;
        }
        else{
            delete [] buf;
            break;
        }
    if(failCount == nTries) return false;

    vector<bool> filled(drive.FragmentCount, false);
    char *serial = new char[drive.MessageSize];
    Fragment Frag;
    for(drive.NextFragmentID = 0; drive.NextFragmentID < drive.FragmentCount; ){
        for(int i = 0; i < FragmentDrive::BatchCount && drive.NextFragmentID + i < drive.FragmentCount; i++){
            if((buf = worker_socket.syncRead((int&)size, 500)) != nullptr && Frag.deserialize(buf, size) && Frag.MessageID == drive.MessageID){
                if(!filled[Frag.FragmentID]) memcpy(serial + Frag.FragmentID * FragmentDrive::MaxFragmentSize, Frag.Payload, Frag.FragmentSize);
                filled[Frag.FragmentID] = true;
                failCount -= failCount > 0;
            }
            else failCount++;
            if(buf != nullptr) delete [] buf;
        }
        while(drive.NextFragmentID < drive.FragmentCount && filled[drive.NextFragmentID]) drive.NextFragmentID++;
        drive.FragmentDriveID++;
        buf = drive.serialize(size);
        failCount += worker_socket.asyncWrite(buf, size) < 0;
        delete [] buf;
        if(failCount >= nTries * FragmentDrive::BatchCount){
            delete [] serial;
            return false;
        }
    }

    if(!obj->deserialize(serial, drive.MessageSize)){
        cout << "A7eh" << endl;
        delete [] serial;
        return false;
    }
    delete [] serial;
    return true;
}

unsigned int WorkerView::getPeerIP() const{
    return worker_socket.getPeerIP();
}
unsigned short WorkerView::getPeerPort() const{
    return worker_socket.getPeerPort();
}

//ClientView
ClientView::ClientView(const char *hostname, short peer_port): client_socket(UDPsocket::resolveHostName(hostname), peer_port){
	//Resovles the host first, then constructs the client socket.
	server_ip = client_socket.getPeerIP();
	server_port = peer_port;
}

bool ClientView::connect(const ServerMessage& msg, const int timeout){
    int size;
    char* buf;
    unsigned int serializedMSGSize;
    char *serializedMSG = msg.serialize(serializedMSGSize);

    if(client_socket.asyncWrite(serializedMSG, serializedMSGSize) < 0){
        delete [] serializedMSG;
        return false;
    }
    delete [] serializedMSG;
    int failCount = 0;
    while(failCount < 10){
        if(timeout == 0) return true;
        if((buf = client_socket.syncRead(size, timeout)) == nullptr){
            //client_socket.releasePeer(server_ip, server_port);
            cout << "LOG::Unable to read or timeout" << endl;
            return false;
        }
        ServerMessage reply;
        if(!reply.deserialize(buf, size)){
            cout << "LOG::Unable to deserialize" << endl;
            failCount++;
            delete [] buf;
            continue;
        }
        delete [] buf;
        if(reply.getID() != msg.getID() + 1 || reply.getVector() != 0xFD && reply.getPayloadSize() != 0){
            cout << "LOG::Invalid packet" << endl;
            failCount++;
            continue;
        }
        break;
    }
    if(failCount == 10) return false;
    client_socket.bindPeer(client_socket.getPeerIP(), client_socket.getPeerPort());
    cout << "LOG::Sucess" << endl;

    return true;
}
void ClientView::disconnect(){
	client_socket.releasePeer(server_ip, server_port);
}
bool ClientView::sendObject(const Transmittable * const obj, const unsigned int nTries){
    char *buf;
    unsigned int size;
    buf = obj->serialize(size);
    if(size == 0){
        delete [] buf;
        return false;
    }
    FragmentDrive drive(size);
    FragmentDrive incDrive;
    vector<Fragment> Frag(drive.FragmentCount);
    for(int i = 0; i < drive.FragmentCount; i++){
        Frag[i].MessageID = drive.MessageID;
        Frag[i].FragmentID = i;
        Frag[i].FragmentSize = (Frag[i].FragmentID + 1 == drive.FragmentCount && size ) ? (size - 1) % FragmentDrive::MaxFragmentSize + 1: FragmentDrive::MaxFragmentSize;
        memcpy(Frag[i].Payload, buf + Frag[i].FragmentID * FragmentDrive::MaxFragmentSize, Frag[i].FragmentSize);
    }
    delete [] buf;
    int failCount = 0;
    buf = drive.serialize(size);
    while(client_socket.asyncWrite(buf, size) < 0 && failCount < nTries) failCount++;
    delete [] buf;
    for(drive.NextFragmentID = 0; drive.NextFragmentID < drive.FragmentCount; ){
        for(int i = 0; i < FragmentDrive::BatchCount && drive.NextFragmentID + i < drive.FragmentCount; i++){
            buf = Frag[drive.NextFragmentID + i].serialize(size);
            failCount += client_socket.asyncWrite(buf, size) < 0;
            delete [] buf;
        }
        while(failCount < nTries*FragmentDrive::BatchCount && (buf = client_socket.syncRead((int&)size, 500)) != nullptr){
            if(incDrive.deserialize(buf, size) && incDrive > drive){
                drive = incDrive;
                failCount -= failCount > 0;
                delete [] buf;
                break;
            }
            failCount++;
            delete [] buf;
        }
        failCount += buf == nullptr;
        if(failCount >= nTries * FragmentDrive::BatchCount) return false;
    }
    return true;
}
bool ClientView::recieveObject(Transmittable * const obj, const unsigned int nTries){
    char *buf;
    unsigned int size;

    FragmentDrive drive;
    int failCount = 0;
    while(failCount < nTries)
        if((buf = client_socket.syncRead((int&)size, 1000)) == nullptr || !drive.deserialize(buf, size)){
            if(buf != nullptr) delete [] buf;
            failCount++;
        }
        else{
            delete [] buf;
            break;
        }
    if(failCount == nTries) return false;

    vector<bool> filled(drive.FragmentCount, false);
    char *serial = new char[drive.MessageSize];
    Fragment Frag;
    for(drive.NextFragmentID = 0; drive.NextFragmentID < drive.FragmentCount; ){
        for(int i = 0; i < FragmentDrive::BatchCount && drive.NextFragmentID + i < drive.FragmentCount; i++){
            if((buf = client_socket.syncRead((int&)size, 500)) != nullptr && Frag.deserialize(buf, size) && Frag.MessageID == drive.MessageID){
                if(!filled[Frag.FragmentID]) memcpy(serial + Frag.FragmentID * FragmentDrive::MaxFragmentSize, Frag.Payload, Frag.FragmentSize);
                filled[Frag.FragmentID] = true;
                failCount -= failCount > 0;
            }
            else failCount++;
            if(buf != nullptr) delete [] buf;
        }
        while(drive.NextFragmentID < drive.FragmentCount && filled[drive.NextFragmentID]) drive.NextFragmentID++;
        drive.FragmentDriveID++;
        buf = drive.serialize(size);
        failCount += client_socket.asyncWrite(buf, size) < 0;
        delete [] buf;
        if(failCount >= nTries * FragmentDrive::BatchCount){
            delete [] serial;
            return false;
        }
    }

    if(!obj->deserialize(serial, drive.MessageSize)){
        delete [] serial;
        return false;
    }
    delete [] serial;
    return true;
}

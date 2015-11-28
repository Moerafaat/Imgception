#ifndef VIEWS_H
#define VIEWS_H

#include <vector>
#include <pthread.h>
#include "UDPsocket.h"
#include "transmittable.h"
using namespace std;

class WorkerView;

class ServerMessage{
    //I didn't put it in message.h because it is not part of the middleware library (Should explain in personal)
    ServerMessage(const ServerMessage&) = delete;
private:
    static unsigned int nextID;
private:
    unsigned int msgID;
    unsigned char msgVector;    //msgVector 0xFD is reserved. It is used to initialize connection between WorkerView and ClientView.
                                //msgVector 0xFE is reserved. It means an acknowledgment message.
                                //msgVector 0xFF is reserved. It means an Error message.
    char *payload;
    unsigned int size;
public:
    ServerMessage(const unsigned char = 0);
    ServerMessage(const unsigned int, const unsigned char);
    ServerMessage(ServerMessage&&);
    ~ServerMessage();

    ServerMessage& operator=(const ServerMessage&);
    ServerMessage& operator=(ServerMessage&&);

    char* serialize(int&) const;
    bool deserialize(const char * const , const unsigned int);

    unsigned int getID() const;
    unsigned char getVector() const;
    const char *getPayload() const;
    const unsigned int getPayloadSize() const;
    void setPayload(const char * const, const unsigned int);
};

class ServerView{
	ServerView() = delete;
	ServerView(const ServerView&) = delete;
	ServerView(ServerView&&) = delete;
    friend class WorkerView;
private:
	static const int MAX_WORKERS = 100;
    bool EXIT_FLAG;
	UDPsocket server_socket;
	WorkerView *workers[MAX_WORKERS];
	vector<WorkerView *> freeWorkers;
	pthread_mutex_t workersLockMutex;
    void (*callbackFunc[256])(WorkerView&, const ServerMessage&);

    WorkerView *lockWorker();
    void unlockWorker(WorkerView *);
public:
	ServerView(unsigned short);
	~ServerView();

    void setCallbackFunc(const unsigned char, void(*)(WorkerView&, const ServerMessage&));
	ServerMessage listen(unsigned int&, unsigned short&);
    void deployWorker(const unsigned int, const unsigned short, const ServerMessage&);
    void cleanExit();
	bool exitStatus() const;
    bool isWorkerFree() const;
};
//getRequest, doOperation, sendReply.

class WorkerView{
	WorkerView() = delete;
	WorkerView(const WorkerView&) = delete;
	WorkerView(WorkerView&&) = delete;
private:
	static void *process (void *); // Process to be spawned in a thread.
	UDPsocket worker_socket;
	pthread_t thread_id;
	pthread_mutex_t available;
	ServerMessage initMsg;
	ServerView *Server;

public:
	WorkerView(ServerView *);
	~WorkerView();

	void deploy(const unsigned int, const unsigned short, const ServerMessage&); //API to deploy thread with IP:Port
    int sendObject(const transmittable * const);
    int recieveObject(transmittable * const, const int = -1);
};
//should provide function for the stuf to use that does the following:
//1-Send/Recieve a whole message. It should execute the serializtion/deserialization proccess and partition collect the message to several packets if needed.
//2-Set message vector call back functions.
//+Check Client for similar functionalites


class ClientView{
	ClientView() = delete;
	ClientView(const ClientView&) = delete;
	ClientView(ClientView&&) = delete;
private:
	UDPsocket client_socket;
	unsigned int server_ip;
	unsigned short server_port;
public:
	ClientView(const char *, short);
	~ClientView() = default;

    unsigned short getMyPort() const;
	unsigned short getPeerPort() const;
	unsigned int getMyIP() const;
	unsigned int getPeerIP() const;

	int connect(const ServerMessage&, const int = -1); //0 timeout = don't wait for worker
	void disconnect();
    int sendObject(const transmittable * const);
    int recieveObject(transmittable * const, const int = -1);
 };
//Should provide functionality to send/recieve message, handel fragmentation and support maybe/atLeastOnce/atMostOnce
//+Check Workers for similar functionalites
#endif

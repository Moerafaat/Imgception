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
    unsigned char msgVector;    //msgVector 0x00 is reserved. It means a Null message. Internally used for checking connectivity and acknowledgments.
                                //msgVector 0xFE is reserved. It means an acknowledgment message.
                                //msgVector 0xFF is reserved. It means an Error message.
    char *payload;
    unsigned int size;
public:
    ServerMessage(const unsigned char = 0);
    ServerMessage(const unsigned int, const unsigned char);
    ServerMessage(const char*);   //Different way of deserializable to emphasize that it part of the middleware and not a typical transmittable.
    ServerMessage(ServerMessage&&);
    ~ServerMessage();

    ServerMessage& operator=(const ServerMessage&);
    ServerMessage& operator=(ServerMessage&&);

    char* serialize(int&) const;

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
	UDPsocket server_socket;
	WorkerView *workers[MAX_WORKERS];
	vector<WorkerView *> freeWorkers;
	pthread_mutex_t workersLockMutex;
	bool EXIT_FLAG;

    WorkerView *lockWorker();
    void unlockWorker(WorkerView *);
public:
	ServerView(unsigned short);
	~ServerView();

	ServerMessage listen(unsigned int&, unsigned short&);
    void deployWorker(const unsigned int, const unsigned short, const ServerMessage&);
    void cleanExit();
	bool exitStatus() const;
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

	bool connect(const ServerMessage&, const unsigned int = 1);    //I removed nTries and added extra parameter here because I think the stub should handel the maybe/atleastonce/atmostonce. The other option is to force all transmittable have some kind of clientID and logical clock associated with them which might require some refactoring if not a lot. Besides I am not sure if we need to make it in this layer or is it okay to make it in the stub. Not sure which is easier.
	void disconnect();
    bool send(const ServerMessage&); //Should be transmittable* but meeh for now.
    ServerMessage recieve();
	//headerMessage *execute(const headerMessage *); // Send an asynchronous message to the server.
 };
//Should provide functionality to send/recieve message, handel fragmentation and support maybe/atLeastOnce/atMostOnce
//+Check Workers for similar functionalites
#endif

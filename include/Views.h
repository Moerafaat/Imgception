#ifndef VIEWS_H
#define VIEWS_H

#include <vector>
#include <pthread.h>
#include "UDPsocket.h"

class WorkerView;
class Transmittable;

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

    char *serialize(unsigned int&) const;
    bool deserialize(const char * const , const int);

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
	static const int MAX_WORKERS = 10;
    bool EXIT_FLAG;
	UDPsocket server_socket;
	WorkerView *workers[MAX_WORKERS];
	std::vector<WorkerView *> freeWorkers;
	pthread_mutex_t workersLockMutex;
    void (*callbackFunc[256])(WorkerView&, const ServerMessage&);
public:
	ServerView(unsigned short);
	~ServerView();

    void setCallbackFunc(const unsigned char, void(*)(WorkerView&, const ServerMessage&));
	ServerMessage listen(unsigned int&, unsigned short&);
    void deployWorker(const unsigned int, const unsigned short, const ServerMessage&);
    void cleanExit();   //Should be called from the same thread as listening. Until a future solution if found
	bool exitStatus() const;
    bool isWorkerFree() const;
private:
    WorkerView *lockWorker();
    void unlockWorker(WorkerView *);
};

class WorkerView{
	WorkerView() = delete;
	WorkerView(const WorkerView&) = delete;
	WorkerView(WorkerView&&) = delete;
private:
	static void *process (void *); // Process to be spawned in a thread.
private:
    UDPsocket worker_socket;
	pthread_t thread_id;
	pthread_mutex_t available;
	ServerMessage initMsg;
	ServerView *Server;
public:
	WorkerView(ServerView *);
	~WorkerView();

	void deploy(const unsigned int, const unsigned short, const ServerMessage&); //API to deploy thread with IP:Port
    bool sendObject(const Transmittable * const, const unsigned int = 5);
    bool recieveObject(Transmittable * const, const unsigned int = 5);

    unsigned int getPeerIP() const;
    unsigned short getPeerPort() const;
};

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

	bool connect(const ServerMessage&, const int = -1); //0 timeout = don't wait for worker
	void disconnect();
    bool sendObject(const Transmittable * const, const unsigned int = 5);
    bool recieveObject(Transmittable * const, const unsigned int = 5);
 };
#endif

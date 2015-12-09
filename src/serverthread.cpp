#include <QDebug>
#include "serverthread.h"
#include "application.h"

ServerThread::ServerThread(Application *P) : ptr(P)
{

}

void ServerThread::run(){
    unsigned int IP;
    unsigned short Port;
    while(!ptr->Server.exitStatus()){
        ServerMessage msg = ptr->Server.listen(IP, Port);
        switch (msg.getVector()) {
        case 0xFF:
            break;
        default:
            ptr->Server.deployWorker(IP, Port, msg);
        }
    }
}

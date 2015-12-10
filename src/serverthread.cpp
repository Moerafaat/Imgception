#include <QDebug>
#include "serverthread.h"
#include "peerprogram.h"

void ServerThread::run(){
    QThread::sleep(5);
    unsigned int IP;
    unsigned short Port;
    while(!PeerProgram::Server.exitStatus()){
        ServerMessage msg = PeerProgram::Server.listen(IP, Port);
        switch (msg.getVector()) {
        case 0xFF:
            break;
        default:
            PeerProgram::Server.deployWorker(IP, Port, msg);
        }
    }
}

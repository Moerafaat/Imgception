#include <QDebug>
#include <iostream>
#include "Views.h"
#include "onlinepeers.h"
using namespace std;

onlinePeers online_list;


void retPeerList(WorkerView& Worker, const ServerMessage& initMsg){
    qDebug() << "I am fucking here bitches!";
    QString key = QString::fromStdString(string(initMsg.getPayload(), initMsg.getPayloadSize()));
    online_list.pushPeer(key, Worker.getPeerIP(), Worker.getPeerPort());
    Worker.sendObject(&online_list);
}

int main(){
    ServerView Server(5000);
    Server.setCallbackFunc(0x01, retPeerList);
    unsigned int fromIP;
    unsigned short fromPort;
    while(!Server.exitStatus()){
        ServerMessage msg = Server.listen(fromIP, fromPort);
        switch(msg.getVector()){
            case 0xFF:
                cout << "LOG::Error while server listening." << endl;
                break;
            default:
                Server.deployWorker(fromIP, fromPort, msg);
        }
    }

    return 0;
}

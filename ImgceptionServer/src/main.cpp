#include <iostream>
#include "Views.h"
#include "onlinepeers.h"
//#include
using namespace std;

onlinePeers online;


void retPeerList(WorkerView& Worker, const ServerMessage& initMsg){

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

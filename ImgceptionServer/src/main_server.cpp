#include <QDebug>
#include <iostream>
#include <QDateTime>
#include "Views.h"
#include "onlinepeers.h"
#include "globals.h"
using namespace std;

static const qint64 set_offline_time = 180000; // Half an hour.
onlinePeers online_list;
QMap<QString, int> key_to_index;
QVector<QDateTime> timestamp;

void refreshOnlineList(){
    onlinePeers new_online_list;
    QMap<QString, int> new_key_to_index;
    QVector<QDateTime> new_timestamp;
    for(int i=0; i<online_list.getPeerCount(); i++){ // Check the timestamps of all peers.
        if(timestamp[i].msecsTo(QDateTime::currentDateTime()) <= set_offline_time){ // Still online.
            // Update online list, index map, and timestamp vector.
            new_key_to_index.insert(online_list.getPeerKey(i).getAsString(), new_online_list.getPeerCount());
            new_timestamp.push_back(timestamp[i]);
            new_online_list.pushPeer(online_list.getPeerKey(i).getAsString(), online_list.getPeerIP(i), online_list.getPeerPort(i));
        }
    }

    online_list = new_online_list;
    key_to_index = new_key_to_index;
    timestamp = new_timestamp;
}

void updateTimeStamp(QString key, const unsigned int IP, const unsigned short Port){
    if(key_to_index.count(key) == 0){ // New entry.
        key_to_index[key] = online_list.getPeerCount();
        online_list.pushPeer(key, IP, Port);
    }
    timestamp.push_back(QDateTime::currentDateTime());
}

void retPeerList(WorkerView& Worker, const ServerMessage& initMsg){
    QString key = QString::fromStdString(string(initMsg.getPayload(), initMsg.getPayloadSize()));

    refreshOnlineList(); // Refresh list upon new request.
    updateTimeStamp(key, Worker.getPeerIP(), Worker.getPeerPort()); // Update timestamp (add peer if it doesn;t exist).
    Worker.sendObject(&online_list); // Send list.
}

int main(){
    Globals::InitFolders();
    ServerView Server(5000);
    Server.setCallbackFunc(P2S_UPDATE_PEER_LIST, retPeerList);
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

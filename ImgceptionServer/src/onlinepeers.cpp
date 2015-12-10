#include "onlinepeers.h"
#include <QDebug>
onlinePeers::onlinePeers() : nPeers(0), peerKey(0), peerIP(0), peerPort(0), peerName(0){

}

onlinePeers& onlinePeers::operator=(const onlinePeers& cpy){
    nPeers = cpy.nPeers;
    peerKey = cpy.peerKey;
    peerIP = cpy.peerIP;
    peerPort = cpy.peerPort;
    peerName = cpy.peerName;
    return *this;
}

bool onlinePeers::pushPeer(QString key, QString NAME, const unsigned int ip, const unsigned short port){
    nPeers++;
    peerKey.push_back(key);
    peerIP.push_back(ip);
    peerPort.push_back(port);
    peerName.push_back(NAME);
    return true;
}
bool onlinePeers::pushPeer(Key& key, QString NAME, const unsigned int ip, const unsigned short port){
    if(key.isPrivate()) return false;
    nPeers++;
    peerKey.push_back(key.getAsString());
    peerIP.push_back(ip);
    peerPort.push_back(port);
    peerName.push_back(NAME);
    return true;
}

unsigned int onlinePeers::getPeerCount() const{
    return nPeers;
}

Key onlinePeers::getPeerKey(unsigned int idx) const{
    if(idx >= nPeers) return Key();
    Key ret;
    if(!ret.setFromString(peerKey[idx])) return Key();
    return ret;
}

unsigned int onlinePeers::getPeerIP(unsigned int idx) const{
    if(idx >= nPeers) return -1;
    return peerIP[idx];
}

unsigned short onlinePeers::getPeerPort(unsigned int idx) const{
    if(idx >= nPeers) return -1;
    return peerPort[idx];
}

QString onlinePeers::getPeerName(unsigned int idx) const{
    if(idx >= nPeers) return "";
    return peerName[idx];
}

char *onlinePeers::serialize(unsigned int& size) const{
    static const unsigned int peerSize = Key::PubKeySize+4+2+4;
    size = 4 + nPeers * (peerSize);
    for(int i = 0; i < peerName.size(); i++) size+=peerName[i].size();
    char *serial = new char[size];
    *(unsigned int*)(serial) = HostToNetwork(nPeers);
    int off = 4;
    for(int i = 0; i < nPeers; i++){
        *(unsigned int*)(serial + off) = HostToNetwork(peerIP[i]); off += 4;
        *(unsigned short*)(serial + off) = HostToNetwork(peerPort[i]); off+= 2;
        memcpy(serial + off, peerKey[i].toStdString().c_str(), Key::PubKeySize); off+=Key::PubKeySize;
        *(int*)(serial + off) = HostToNetwork(peerName[i].size()); off+=4;
        memcpy(serial+off, peerName[i].toStdString().c_str(), peerName[i].size()); off+=peerName[i].size();
    }
    return serial;
}

bool onlinePeers::deserialize(const char * const buf, const unsigned int size){
    static const unsigned int peerSize = Key::PubKeySize+4+2+4;
    if(size < 4) return false;
    nPeers = NetworkToHost(*(unsigned int*)buf);
    if(size < 4 + nPeers * peerSize) return false;
    int knownSize = 4 + nPeers * peerSize;
    int off = 4;
    for(int i = 0; i < nPeers; i++){
        peerIP.push_back(NetworkToHost(*(unsigned int*)(buf + off))); off+=4;
        peerPort.push_back(NetworkToHost(*(unsigned short*)(buf + off))); off+=2;
        peerKey.push_back(QString::fromStdString(std::string(buf + off, Key::PubKeySize))); off += Key::PubKeySize;
        int nameSize = NetworkToHost(*(int*)(buf+off)); off+=4;
        knownSize += nameSize; qDebug() << nameSize;
        //if(size < knownSize) return false;
        peerName.push_back(QString::fromStdString(std::string(buf + off, nameSize)));
        off += nameSize;
    }
    return knownSize == size;
}

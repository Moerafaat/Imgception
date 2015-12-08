#include "onlinepeers.h"

onlinePeers::onlinePeers() : nPeers(0), peerKey(0), peerIP(0), peerPort(0){

}

bool onlinePeers::pushPeer(QString key, const unsigned int ip, const unsigned short port){
    nPeers++;
    peerKey.push_back(key);
    peerIP.push_back(ip);
    peerPort.push_back(port);
    return true;
}
bool onlinePeers::pushPeer(Key& key, const unsigned int ip, const unsigned short port){
    if(key.isPrivate()) return false;
    nPeers++;
    peerKey.push_back(key.getAsString());
    peerIP.push_back(ip);
    peerPort.push_back(port);
    return true;
}

unsigned int onlinePeers::getPeerCount() const{
    return nPeers;
}

Key onlinePeers::getPeerKey(unsigned int idx) const{
    if(idx >= nPeers) return Key();
    Key ret;
    ret.setFromString(peerKey[idx]);
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

char *onlinePeers::serialize(unsigned int& size) const{
    static const unsigned int peerSize = Key::PubKeySize+4+2;
    size = 4 + nPeers * (peerSize);
    char *serial = new char[size];
    *(unsigned int*)(serial) = HostToNetwork(nPeers);
    for(int i = 0; i < nPeers; i++){
        *(unsigned int*)(serial + 4 + i * peerSize) = HostToNetwork(peerIP[i]);
        *(unsigned short*)(serial + 8 + i * peerSize) = HostToNetwork(peerPort[i]);
        memcpy(serial + 10 + i * peerSize, peerKey[i].toStdString().c_str(), Key::PubKeySize);
    }
    return serial;
}

bool onlinePeers::deserialize(const char * const buf, const unsigned int size){
    static const unsigned int peerSize = Key::PubKeySize+4+2;
    if(size < 4) return false;
    nPeers = NetworkToHost(*(unsigned int*)buf);
    if(size != 4 + nPeers * peerSize) return false;
    for(int i = 0; i < nPeers; i++){
        peerIP.push_back(NetworkToHost(*(unsigned int*)(buf + 4 + i * peerSize)));
        peerPort.push_back(NetworkToHost(*(unsigned short*)(buf + 8 + i * peerSize)));
        peerKey.push_back(QString::fromStdString(std::string(buf + 10 + i * peerSize, Key::PubKeySize)));
    }
    return true;
}

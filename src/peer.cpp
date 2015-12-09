#include "peer.h"
#include "key.h"

Peer::Peer(QString Key, QString Name, bool IsOnline, unsigned int Ip, unsigned short Port){
    key.setFromString(Key);
    name = Name;
    online = IsOnline;
    IP = Ip;
    port = Port;
}

Image Peer::getImageByIndex(unsigned int idx){
    return images[image_key_to_index[idx]];
}

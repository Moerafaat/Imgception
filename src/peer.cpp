#include "peer.h"
#include "key.h"

Peer::Peer(Key K, QString Name, bool IsOnline, unsigned int Ip, unsigned short Port){
    key = K;
    name = Name;
    online = IsOnline;
    IP = Ip;
    port = Port;
}

Image Peer::getImageByIndex(unsigned int idx){
    return image_list[image_key_to_index[idx]];
}

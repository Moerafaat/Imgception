#include "notification.h"

int Notification::nextID = 0;

Notification::Notification(Key SenderKey, Key ReceiverKey)
    : sender_key(SenderKey), receiver_key(ReceiverKey){
    generateID();
}

unsigned int Notification::generateID(){
    return ID = nextID++;
}

void Notification::setPayload(const char* Payload, const unsigned int Size, bool PayloadType){
    payload = new char[Size];
    memcpy(payload, Payload, Size);
    payload_type = PayloadType;
    payload_size = Size;
}

char* Notification::serialize(unsigned int & Size) const{
    Size = 4 + Key::PubKeySize + Key::PubKeySize + payload_size + payload_size + 1 + 4;
    char *serial = new char[Size];
    *(unsigned int*)(serial) = HostToNetwork(ID);
    memcpy(serial + 4, sender_key.getAsString().toStdString().c_str(), Key::PubKeySize);
    memcpy(serial + 4 + Key::PubKeySize, receiver_key.getAsString().toStdString().c_str(), Key::PubKeySize);
    *(unsigned int*)(serial + 4 + Key::PubKeySize + Key::PubKeySize) = HostToNetwork(payload_size);
    memcpy(serial + 4 + Key::PubKeySize + Key::PubKeySize + 4, payload, payload_size);
    *(serial + 4 + Key::PubKeySize + Key::PubKeySize + 4 + payload_size) = payload_type;
    return serial;
}

bool Notification::deserialize(const char* const SerializedNotification, const unsigned int Size){
    ID = NetworkToHost(*(unsigned int*)SerializedNotification);
    sender_key.setFromString(QString::fromStdString(std::string(SerializedNotification + 4, Key::PubKeySize)));
    receiver_key.setFromString(QString::fromStdString(std::string(SerializedNotification + 4 + Key::PubKeySize, Key::PubKeySize)));
    payload_size = NetworkToHost(*(unsigned int*)(SerializedNotification + 4 + Key::PubKeySize + Key::PubKeySize));
    if(Size != 4 + Key::PubKeySize + Key::PubKeySize + payload_size + payload_size + 1 + 4) return false;
    payload = new char[payload_size];
    memcpy(payload, SerializedNotification + 4 + Key::PubKeySize + Key::PubKeySize + 4, payload_size);
    payload_type = *(SerializedNotification + 4 + Key::PubKeySize + Key::PubKeySize + 4 + payload_size);
    return true;
}


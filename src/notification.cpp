#include "notification.h"

Notification::Notification(Key SenderKey, Key ReceiverKey, char *Payload, bool PayloadType)
    : sender_key(SenderKey), receiver_key(ReceiverKey), payload(Payload), payload_type(PayloadType){
}

void Notification::setPayload(const char* Payload, const unsigned int Size, bool PayloadType){
    memcpy(payload, Payload, Size);
    payload_type = PayloadType;
    payload_size = Size;
}

char* Notification::serialize(unsigned int & Size) const{
    Size = Key::PubKeySize + Key::PubKeySize + payload_size + payload_size + 1 + 4;
    char *serial = new char[Size];
    memcpy(serial, sender_key.getAsString().toStdString().c_str(), Key::PubKeySize);
    memcpy(serial + Key::PubKeySize, receiver_key.getAsString().toStdString().c_str(), Key::PubKeySize);
    *(unsigned int*)(serial + Key::PubKeySize + Key::PubKeySize) = HostToNetwork(payload_size);
    memcpy(serial + Key::PubKeySize + Key::PubKeySize + 4, payload, payload_size);
    *(serial + Key::PubKeySize + Key::PubKeySize + 4 + payload_size) = payload_type;
    return serial;
}

bool Notification::deserialize(const char* const SerializedNotification, const unsigned int Size){
    sender_key = QString::fromStdString(std::string(SerializedNotification, Key::PubKeySize));
    receiver_key = QString::fromStdString(std::string(SerializedNotification + Key::PubKeySize, Key::PubKeySize));
    payload_size = NetworkToHost(*(unsigned int*)(SerializedNotification + Key::PubKeySize + Key::PubKeySize));
    if(Size != Key::PubKeySize + Key::PubKeySize + payload_size + payload_size + 1 + 4) return false;
    payload = new char[payload_size];
    memcpy(payload, SerializedNotification + Key::PubKeySize + Key::PubKeySize + 4, payload_size);
    payload_type = *(SerializedNotification + Key::PubKeySize + Key::PubKeySize + 4 + payload_size);
    return true;
}


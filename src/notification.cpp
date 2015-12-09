#include "notification.h"

Notification::Notification(RSA SenderKey, RSA ReceiverKey, char *Payload, int, bool PayloadType){

}

char* Notification::serialize(unsigned int & Size) const{
    return nullptr;
}

bool Notification::deserialize(const char* const SerializedNotification, const unsigned int Size){
    return false;
}


#include "notificationheader.h"

NotificationHeader::NotificationHeader(int NumberOfNotifications){

}

char* NotificationHeader::serialize(unsigned int & Size) const{
    return nullptr;
}

bool NotificationHeader::deserialize(const char* const SerializedNotificationHeader, const unsigned int Size){
    return false;
}

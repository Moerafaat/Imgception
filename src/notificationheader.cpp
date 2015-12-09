#include "notificationheader.h"

NotificationHeader::NotificationHeader(unsigned int NumberOfNotifications): number_of_notifications(NumberOfNotifications){

}

char* NotificationHeader::serialize(unsigned int & Size) const{
    Size = 4;
    char *serial = new char[Size];
    *(unsigned int*)(serial) = HostToNetwork(number_of_notifications);
    return serial;
}

bool NotificationHeader::deserialize(const char* const SerializedNotificationHeader, const unsigned int Size){
    if(Size != 4) return false;
    number_of_notifications = NetworkToHost(*(unsigned int*)SerializedNotificationHeader);
    return true;
}

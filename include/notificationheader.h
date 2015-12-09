#ifndef NOTIFICATIONHEADER_H
#define NOTIFICATIONHEADER_H


#include "transmittable.h"

class NotificationHeader : public Transmittable{
public:
    NotificationHeader() = default;
    NotificationHeader(const NotificationHeader&) = default;
    NotificationHeader(NotificationHeader&&) = default;
    NotificationHeader(unsigned int);

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);

    unsigned int number_of_notifications; // Number of notifications that will be recieved.
};

#endif // NOTIFICATIONHEADER_H

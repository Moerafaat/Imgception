#ifndef NOTIFICATIONHEADER_H
#define NOTIFICATIONHEADER_H


#include "transmittable.h"

class NotificationHeader : public Transmittable{
public:
    NotificationHeader(int);

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);
private:
    int number_of_notifications; // Number of notifications that will be recieved.
};

#endif // NOTIFICATIONHEADER_H

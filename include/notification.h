#ifndef NOTIFICATION_H
#define NOTIFICATION_H


#include "transmittable.h"
#include "key.h"

class Notification : public Transmittable{
public:
    Notification() = default;
    Notification(const Notification&) = default;
    Notification(Notification&&) = default;
    Notification(Key, Key);
    ~Notification() = default;

    void setPayload(const char*, const unsigned int, bool);
    unsigned int generateID();

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);

    static int nextID;

    unsigned int ID; // Notification Identifier.
    Key sender_key; // Notification sender.
    Key receiver_key; // Notification receiver.
    unsigned int payload_size;
    char*payload; // Send either image or update.
    bool payload_type; // 0 means image, 1 means update.
};

#endif // NOTIFICATION_H

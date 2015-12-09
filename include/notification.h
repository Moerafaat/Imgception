#ifndef NOTIFICATION_H
#define NOTIFICATION_H


#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "transmittable.h"

class Notification : public Transmittable{
public:
    Notification() = default;
    Notification(const Notification&) = default;
    Notification(Notification&&) = default;
    Notification(RSA, RSA, char *,int ,bool);
    ~Notification() = default;

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);
private:
    RSA sender_key; // Notification sender.
    RSA receiver_key; // Notification receiver.
    char*payload; // Send either image or update.
    int payloadSize; //
    bool payload_type; // 0 means image, 1 means update.
};

#endif // NOTIFICATION_H

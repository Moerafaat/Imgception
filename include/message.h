#ifndef MESSAGE_H
#define MESSAGE_H

#include "transmittable.h"

enum header_type{
    mtQUIT  = 0xFF,
    mtNULL  = 0x00,
    mtACK   = 0x01,
    mtMSG   = 0x02,
};

/*class headerMessage : public transmittable{
    headerMessage(const headerMessage&) = delete;
public:    //Maybe getters
    unsigned char headerType;
    unsigned long long fromID;
private:
    unsigned int msgSize;   //Thought we might need them to send extra information (for expandability) but maybe I was wrong. Remove if not needed.
    char *msg;
public:
    headerMessage();
    headerMessage(headerMessage&&);
    headerMessage(const unsigned char, const unsigned long long);
    ~headerMessage();

    virtual char *serialize(unsigned int &) const;
    virtual void deserialize(const char * const, const unsigned int);

    const char *getMessage() const;
    const unsigned int getMessageSize() const;
    void setMessage(const char * const, const unsigned int);
    void clearMessage();
};*/

#endif

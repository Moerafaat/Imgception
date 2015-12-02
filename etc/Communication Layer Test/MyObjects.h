#ifndef MYOBJECTS_H
#define MYOBJECTS_H

#include <string>
#include "transmittable.h"

class MyString : public transmittable{  //Test Class
public:
    std::string str;
public:
    MyString() = default;
    MyString(const std::string&);

    char *serialize(unsigned int&) const;
    bool deserialize(const char * const, const unsigned int);
};

class Acknowledgment : public transmittable{
public:
    bool ack;
public:
    Acknowledgment(bool = true);

    char *serialize(unsigned int&) const;
    bool deserialize(const char * const, const unsigned int);
};

#endif

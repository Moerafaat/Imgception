#ifndef TRANSMITTABLE_H
#define TRANSMITTABLE_H

// Will be inherited: To use serialization.
class Transmittable{
public:
    transmittable() = default;
    transmittable(const transmittable&) = default;
    transmittable(transmittable&&) = default;
    ~transmittable() = default;

    virtual char *serialize(unsigned int&) const = 0;
    virtual bool deserialize(const char * const, const unsigned int) = 0;
};

unsigned char NetworkToHost(const unsigned char);
unsigned short NetworkToHost(const unsigned short);
unsigned int NetworkToHost(const unsigned int);
unsigned long NetworkToHost(const unsigned long);
unsigned long long NetworkToHost(const unsigned long long);
char NetworkToHost(const char);
short NetworkToHost(const short);
int NetworkToHost(const int);
long NetworkToHost(const long);
long long NetworkToHost(const long long);

unsigned char HostToNetwork(const unsigned char);
unsigned short HostToNetwork(const unsigned short);
unsigned int HostToNetwork(const unsigned int);
unsigned long HostToNetwork(const unsigned long);
unsigned long long HostToNetwork(const unsigned long long);
char HostToNetwork(const char);
short HostToNetwork(const short);
int HostToNetwork(const int);
long HostToNetwork(const long);
long long HostToNetwork(const long long);

#endif

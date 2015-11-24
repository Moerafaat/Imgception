#include "transmittable.h"
#include <arpa/inet.h>

unsigned char NetworkToHost(const unsigned char val){
    return val;
}
unsigned short NetworkToHost(const unsigned short val){
    return ntohs(val);
}
unsigned int NetworkToHost(const unsigned int val){
    return ntohl(val);
}
unsigned long NetworkToHost(const unsigned long val){
    return ntohl(val);
}
unsigned long long NetworkToHost(const unsigned long long val){ //From BigEndian Regardless (until we find a portable solution)
    const unsigned int sz = sizeof(val) / sizeof(char);
    const char *temp = (const char*)&val;
    unsigned long long ret ;
    for(int i = 0; i < sz; i++){
        ret <<= sizeof(char) << 3;
        ret |= temp[i] & ((1 << (sizeof(char) << 3)) - 1);
    }
    return ret;
}
char NetworkToHost(const char val){
    return val;
}
short NetworkToHost(const short val){
    return ntohs(val);
}
int NetworkToHost(const int val){
    return ntohl(val);
}
long NetworkToHost(const long val){
    return ntohl(val);
}
long long NetworkToHost(const long long val){ //From BigEndian Regardless (until we find a portable solution)
    const unsigned int sz = sizeof(val) / sizeof(char);
    const char *temp = (const char*)&val;
    long long ret ;
    for(int i = 0; i < sz; i++){
        ret <<= sizeof(char) << 3;
        ret |= temp[i] & ((1 << (sizeof(char) << 3)) - 1);
    }
    return ret;
}

unsigned char HostToNetwork(const unsigned char val){
    return val;
}
unsigned short HostToNetwork(const unsigned short val){
    return htons(val);
}
unsigned int HostToNetwork(const unsigned int val){
    return htonl(val);
}
unsigned long HostToNetwork(const unsigned long val){
    return htonl(val);
}
unsigned long long HostToNetwork(const unsigned long long val){ //To BigEndian Regardless (until we find a portable solution)
    const unsigned int sz = sizeof(val) / sizeof(char);
    char temp[sz];
    unsigned long long tval = val;
    for(int i = 0; i < sz; i++, tval >>= sizeof(char) << 3)
        temp[sz - 1 - i] = tval & ((1 << (sizeof(char) << 3)) - 1);
    return *(unsigned long long*)temp;
}
char HostToNetwork(const char val){
    return val;
}
short HostToNetwork(const short val){
    return htons(val);
}
int HostToNetwork(const int val){
    return htonl(val);
}
long HostToNetwork(const long val){
    return htonl(val);
}
long long HostToNetwork(const long long val){ //To BigEndian Regardless (until we find a portable solution)
    const int sz = sizeof(val) / sizeof(char);
    char temp[sz];
    long long tval = val;
    for(int i = 0; i < sz; i++, tval >>= sizeof(char) << 3)
        temp[sz - 1 - i] = tval & ((1 << (sizeof(char) << 3)) - 1);
    return *(long long*)temp;
}

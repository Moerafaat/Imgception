#include <cstring>
#include "MyObjects.h"
using namespace std;

MyString::MyString(const string& s){
    str = s;
}

char *MyString::serialize(unsigned int& size) const{
    char *buf = new char[size = 4 + str.size()];
    *(unsigned int*)buf = HostToNetwork(str.size());
    memcpy(buf+4, str.c_str(), str.size());
    return buf;
}
bool MyString::deserialize(const char * const buf, const unsigned int sz){
    if(sz < 4) return false;
    int size = NetworkToHost(*(unsigned int*)buf);
    if(size + 4 != sz) return false;
    str.assign(buf + 4, size);
    return true;
}

Acknowledgment::Acknowledgment(bool f){
    ack = f;
}

char* Acknowledgment::serialize(unsigned int& size) const{
    size = 1;
    char *buf = new char;
    *buf = (char) ack;
    return buf;
}

bool Acknowledgment::deserialize(const char * const buf, const unsigned int sz){
    if(sz != 1) return false;
    ack = (bool)*buf;
    return true;
}

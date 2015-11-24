#include <cstring>
#include "message.h"

headerMessage::headerMessage() : headerType(0), fromID(0), msgSize(0), msg(nullptr){

}
headerMessage::headerMessage(headerMessage&& cpy) : headerType(cpy.headerType), fromID(cpy.fromID), msgSize(cpy.msgSize), msg(cpy.msg){
    cpy.msg = nullptr;
}
headerMessage::headerMessage(const unsigned char type, const unsigned long long ID) : headerType(type), fromID(ID), msgSize(0), msg(nullptr){

}
headerMessage::~headerMessage(){
    if(msg != nullptr) delete [] msg;
}

const char *headerMessage::getMessage() const{
    return msg;
}
const unsigned int headerMessage::getMessageSize() const{
    return msgSize;
}
void headerMessage::setMessage(const char * const buffer, const unsigned int size){
    msgSize = size;
    msg = new char[msgSize];
    memcpy(msg, buffer, msgSize);
}
void headerMessage::clearMessage(){
    if(msg != nullptr) delete [] msg;
    msg = nullptr;
    msgSize = 0;
}

char *headerMessage::serialize(unsigned int& sz) const{
    sz = sizeof(headerType) + sizeof(fromID) + sizeof(msgSize) + msgSize;
    char *buffer = new char[sz];
    int off = 0;
    *(unsigned char*)       (buffer + off) = HostToNetwork(headerType);  off += sizeof(headerType);
    *(unsigned long long*)  (buffer + off) = HostToNetwork(fromID);      off += sizeof(fromID);
    *(unsigned int*)        (buffer + off) = HostToNetwork(msgSize);     off += sizeof(msgSize);
    memcpy(buffer + off, msg, msgSize);
    return buffer;
}

void headerMessage::deserialize(const char * const buffer, const unsigned int sz){
    if(sz < sizeof(headerType) + sizeof(fromID) + sizeof(msgSize)) throw("Error deserializing: incomplete message");
    int off = 0;
    headerType  = NetworkToHost(*(unsigned char*)       (buffer + off));   off += sizeof(headerType);
    fromID      = NetworkToHost(*(unsigned long long*)  (buffer + off));   off += sizeof(fromID);
    msgSize     = NetworkToHost(*(unsigned int*)        (buffer + off));   off += sizeof(msgSize);
    if(msgSize != 0) {
        msg = new char[msgSize];
        memcpy(msg, buffer + off, msgSize);
    }
}

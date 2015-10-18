#include <cstring>
#include "message.h"

message::message(void *payload, int size): msg(payload), msg_size(size){

}

message::message(const message &MSG): msg_size(MSG.msg_size), msg(MSG.msg) {

}

message::message(message &&MSG): msg_size(MSG.msg_size), msg(MSG.msg){

}

message::~message(){

}

const void *message::getMessage() const{
	return msg;
}

int message::getMessageSize() const{
	return msg_size;
}

// Constructors need to be fixed.

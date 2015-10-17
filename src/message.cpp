#include "message.h"

message::message(void *payload, int size): msg(payload), msg_size(size){

}

message::~message(){

}

const void *message::getMessage() const{
	return msg;
}

int message::getMessageSize() const{
	return msg_size;
}
#ifndef MESSAGE_H
#define MESSAGE_H

class message{
	message() = delete;
	message(const message&) = delete;
	message(message&&) = delete;
private:
	const void *msg;
	const int msg_size;

public:
	message(void *, int);
	~message();

	const void *getMessage() const;
	int getMessageSize() const;
};

#endif
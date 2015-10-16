#ifndef MESSAGE_H
#define MESSAGE_H

class message {
	message() = delete;
	message(const message&) = delete;
	message(message&&) = delete;
private:
	const void *msg;
	const size_t msg_size;

public:
	message(void *, size_t);
	~message();

	const void *getMessage() const;
	size_t getMessageSize() const;
};

#endif
#include <iostream>
#include "client.h"

using namespace std;

int main(int argc, char *argv[]){
	cout << "Hello From Client!" << endl;

	client Client("localhost", 5000); // Construct a client.
	char str[] = "Hi Server. This is a Client!"; // Test data to send to server.
	message msg(str, sizeof(str)); // Construct message object.
	Client.execute(&msg); // Send the client data to the server.
	return 0;
}

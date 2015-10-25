#include <iostream>
#include "client.h"

using namespace std;

int main(int argc, char *argv[]){
	cout << "Hello From Client!" << endl;

	client Client("localhost", 5000); // Construct a client.
	Client.connect();
	string str;
	while(getline(cin, str)) {
		message msg(&str, sizeof(str)); // Construct message object.
		Client.execute(&msg); // Send the client data to the server.
	}
	Client.disconnect();
	return 0;
}

#include <iostream>
#include "client.h"

using namespace std;

int main(int argc, char *argv[]){
	cout << "Hello From Client!" << endl;

	client Client("localhost", 5000); // Construct a client.
	if(Client.connect() == -1){
		cout << "Unable to connect to server" << endl;
		return 0;
	}
	string str;
	while(getline(cin, str)) {
		message msg(&str, sizeof(str)); // Construct message object.
		Client.execute(&msg); // Send the client data to the server.
		if(str == "q") break;
	}
	Client.disconnect();
	return 0;
}

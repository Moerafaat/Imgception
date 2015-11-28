#include <iostream>
#include <string>
#include "Views.h"
#include "MyObjects.h"
using namespace std;

//Stub

//"10.40.40.185"
ClientView Client("localhost", 5000); // Construct a client.

bool RemotePrint(MyString str){
    if(Client.connect(ServerMessage(0x01), 5000) == -1){
        cout << "Unable to connect with server worker" << endl;
        return false;
    }
    //Send message
    if(Client.sendObject(&str)){
        cout << "Unable to send message" << endl;
        Client.disconnect();
        return false;
    }
    Acknowledgment a;
    if(Client.recieveObject(&a, 10000) == -1 || !a.ack){
        cout << "Message not acknowged." << endl;
        Client.disconnect();
        return false;
    }
    
    Client.disconnect();
    return true;
}
bool RemoteExitServer(){
    if(Client.connect(ServerMessage(0x00), 0) == -1)
        return false;

    Client.disconnect();
    return true;
}

//End of stub

int main(int argc, char *argv[]){
    string str;
	while(getline(cin, str)) {
        if(str == "q"){
            if(RemoteExitServer()){ cout << "LOG::Server is Exiting..." << endl; break;}
            else cout << "LOG::Remote Exit failed!" << endl;
        }
        else{
            if(RemotePrint(MyString(str))) cout << "LOG::Message delievered" << endl;
            else cout << "LOG::Message delievary failed!" << endl;
        }
	}
	return 0;
}

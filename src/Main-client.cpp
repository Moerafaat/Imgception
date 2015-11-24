#include <iostream>
#include <string>
#include "Views.h"
using namespace std;

//Stub

//"10.40.40.185"
ClientView Client("localhost", 5000); // Construct a client.

bool RemotePrint(string str){
    if(!Client.connect(ServerMessage(0x01))){
        cout << "Unable to connect with server" << endl;
        return false;
    }
    //Send message
    ServerMessage msg(0x02);
    msg.setPayload(str.c_str(), str.size() + 1);
    if(!Client.send(msg)){
        cout << "Unable to send message" << endl;
        return false;
    }
    msg = Client.recieve();
    Client.disconnect();
    return msg.getVector() == 0xFE;
}
bool RemoteExitServer(){
    
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
            if(RemotePrint(str)) cout << "LOG::Message delievered" << endl;
            else cout << "LOG::Message delievary failed!" << endl;
        }
	}
	return 0;
}

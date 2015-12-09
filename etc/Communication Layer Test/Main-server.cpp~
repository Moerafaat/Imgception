#include <iostream>
#include <string>
#include <cstring>
#include "Views.h"
#include "MyObjects.h"
using namespace std;

//Server Stub Maybe?
void PrintMessage(WorkerView& Worker, const ServerMessage& initMsg){
    MyString str;
    if(!Worker.recieveObject(&str)){
        cout << "Error while recieveing MyString." << endl;
        return;
    }

    //Sending acknowledgment
    Acknowledgment a(true);
    if(!Worker.sendObject(&a)){
        cout << "Error while sending Acknowledgment." << endl;
        return;
    }

    cout << str.str << endl;
}
//End of ServerStub

int main(int argc, char *argv[]){
    ServerView Server(5000);
    Server.setCallbackFunc(0x01, PrintMessage);
    char c; cin >> c;   //Just to help testing dropped packets
    cout << "LOG::Server is up" << endl;
    unsigned int fromIP;
    unsigned short fromPort;
    while(!Server.exitStatus()){  // Construct the server and call the listen function.
        ServerMessage msg = Server.listen(fromIP, fromPort);
        switch(msg.getVector()){
            case 0xFF:
                cout << "LOG::Error while server listening!" << endl;
                cout << "LOG::Shutting down server..." << endl;
                Server.cleanExit();
            break;
            case 0x00:
                cout << "LOG::NULL message recieved!" << endl;
                cout << "LOG::Shutting down server..." << endl;
                Server.cleanExit();
            break;
            default:
                Server.deployWorker(fromIP, fromPort, msg);
            break;
        }
    }
    cout << "LOG::Server is shutdown." << endl;

	return 0;
}

/*
imgception
    :MyKeyFile
        -KeyFile
        -Name
    :PeerKeyFile
        -Key1 Name
    <Key1>
    <Key2>
    <Key2>
        :Header
            -img1 Path img1name img1 size img1
            -img1 Path img1name img1 size img1
        :img1.png
        :img2.png
*/

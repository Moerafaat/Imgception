#include <iostream>
#include "Views.h"

using namespace std;

int main(int argc, char *argv[]){
    //ServerPRogram:
    //Init Stub:
    //  set for callbacks

    ServerView Server(5000);
    char c; cin >> c;
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

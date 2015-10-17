#include <iostream>
#include "client.h"

using namespace std;

int main(int argc, char *argv[]){
	cout << "Hello From Client!" << endl;
	
	client Client("localhost", 5000);
	char str[] = "Hi Server. This is a Client!";
	message msg(str, sizeof(str));
	Client.execute(&msg);
	return 0;
}
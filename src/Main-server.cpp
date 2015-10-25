#include <iostream>
#include "server.h"

using namespace std;

int main(int argc, char *argv[]){
	cout << "Hello From Server!" << endl;
try{
	server("localhost", 5000).listen(); // Construct the server and call the listen function.
}
catch(const char* str){
	cout << str << endl;
}

	return 0;
}

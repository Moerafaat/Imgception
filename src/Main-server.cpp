#include <iostream>
#include "server.h"

using namespace std;

int main(int argc, char *argv[]){
	cout << "Hello From Server!" << endl;
	
	server("localhost", 5000).listen();
	return 0;
}
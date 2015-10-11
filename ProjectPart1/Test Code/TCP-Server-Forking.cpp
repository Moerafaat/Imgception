#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
}; */

void terminate_with_error (const char* error_msg, int sock){
	perror("Error Binding Socket: ");
	if(sock != -1){
		close(sock);
		exit(1);
	}
}

int main(){
	int sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	socklen_t sin_size = sizeof(struct sockaddr_in);

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) terminate_with_error("Error Creating Socket", sock);
	int sock_opt = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt,sizeof (sock_opt));
	
	// Server details.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5000);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serverAddr.sin_zero), 8);

	if(bind(sock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1) terminate_with_error("Error Binding", sock); // Binding call.

	if(listen(sock, 10) == -1) terminate_with_error("Error Listening: ", sock); // Listening for incoming requests.

	while(1){ // Infinite Loop for server to keep listening.
		int newsock = accept(sock, (struct sockaddr *)&clientAddr, &sin_size);
		if(newsock < 0) terminate_with_error("Error Accepting Socket", 0);
		else{
			int pid = fork(); // Forking a worker to serve the client.
			if(pid < 0) terminate_with_error("Error on Fork: ", 0);
			else if(pid == 0){ // Child process.
				int maxBytes = 1024;
				char buffer[maxBytes];
				memset(buffer, 0, maxBytes);
				int byte_read = recv(newsock, buffer, maxBytes, 0);
				if(byte_read > 0) printf("Received Message From %s:%d\n%s\n", (char *)inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port, buffer);
				else perror("Error Receiving Message: ");
				close(newsock);
			}
			else{ // Parent process.
				close(newsock);
			}
		}
	}	
}
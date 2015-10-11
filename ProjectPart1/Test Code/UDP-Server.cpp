#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void terminate_with_error (const char * error_msg,int sock){
    perror("Error Binding Socket:");
    if(sock != -1){
    	close (sock);
        exit(1);
    }
}

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) terminate_with_error("Error Creating Socket",sock);
    int sock_opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt,sizeof (sock_opt));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5001);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serverAddr.sin_zero), 8);
    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1) terminate_with_error("Error Binding",sock);

    int maxBytes = 1024;
    char buffer[maxBytes];
    memset(buffer,0,maxBytes);
	int bytes_read = recvfrom(sock,buffer,maxBytes,0,(struct sockaddr *) &clientAddr,&sin_size);

	if (bytes_read > 0) printf ("Received Message from %s:%d\n%s\n",(char *)inet_ntoa(clientAddr.sin_addr),clientAddr.sin_port,buffer);
    else perror("Error Receiving Message:");
	close(sock);
}
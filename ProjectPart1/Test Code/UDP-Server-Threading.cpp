#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

const int maxBytes = 1024; // Maximum size of the buffer.

struct client { // Struct created for passing parameters to the worker function through the thread.
    char buffer[maxBytes];
    struct sockaddr_in clientAddr;
};

void *process (void *);

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
    
    printf("Here");
    char buffer[maxBytes];
    while(1){ // Server keeps receiving messages from clients.
        memset(buffer,0,maxBytes);
    	int byte_read = recvfrom(sock,buffer,maxBytes,0,(struct sockaddr *) &clientAddr,&sin_size); // Blocking call.
        if(byte_read <= 0) perror("Error Receiving Message: ");
        else{
            struct client client_data;
            strncpy(client_data.buffer, buffer, maxBytes);
            client_data.clientAddr = clientAddr;

            pthread_t thread_id;
            if(pthread_create(&thread_id, NULL, process, (void*) &client_data) < 0){ // Spawn a worker thread to handle the client request.
                perror("Error Creating Thread.");
                return 1;
            }
        }
    }
}

void *process (void *client_data){ // Worker function to handle the client request.
    struct client data = *(struct client*)client_data;
    printf("Received Message From %s:%d\n%s\n", (char *)inet_ntoa(data.clientAddr.sin_addr), data.clientAddr.sin_port, data.buffer);
}
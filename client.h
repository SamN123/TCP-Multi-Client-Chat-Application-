#ifndef CLIENT_H
#define CLIENT_H

// Libraries used
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Constants defined for client use 
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

//Client Function Declarations 

// Main level client function handler 
void runClient(void);

// Function to create a socket and connect to the server 
int connectToServer(void);

// Function to send and receive messages 
void sendMessages(int serverFd);

#endif

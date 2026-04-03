#ifndef SERVER_H
#define SERVER_H

// server libraries used 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// server constants defined 
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define BACKLOG 10


// main level server function 
void runServer(void);

// creates listening socket for client requests 
int createServerSocket(void);

// handle client manages all the clients and messages 
void handleClients(int listenFd);

// function to add new client in array 
int addClient(int clients[], int newSocket);

// function that removes client from array 
void removeClient(int clients[], int index);

#endif

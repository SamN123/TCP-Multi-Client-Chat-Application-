#ifndef SERVER_H
#define SERVER_H

#ifndef SERVER_H
#define SERVER_H

/* Libraries needed for server functions */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/* CONSTANT VALUES */
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define BACKLOG 10

/*Function Declarations */

/* server entry */
void run_server(void);

/* Prepare the listening socket */
int create_server_socket(void);

/* Main server loop */
void run_server_loop(int listen_fd);

/* Store clients */
void add_client(int client_sockets[], int new_fd);

/* Function to remove client */
void remove_client(int client_sockets[], int index);

#endif



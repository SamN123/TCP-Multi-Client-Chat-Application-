#ifndef CLIENT_H
#define CLIENT_H

/*libraries used */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*CONSTANT VALUES USED */
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

/*Function Declarations*/

/* Entry level */
void run_client(void);

/* Create socket and connect  */
int connect_to_server(void);

/* Send user input and revieve echo reply back*/
void run_client_loop(int server_fd);

#endif

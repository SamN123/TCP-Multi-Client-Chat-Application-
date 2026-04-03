#include "server.h"

// runServer( ) builds the listening socket and hands control to 
// handleClients( ) function 
void runServer(void)
{
    int listenFd = createServerSocket();

    printf("Server started on port %d...\n", SERVER_PORT);

    handleClients(listenFd);

    close(listenFd);
}

/* =========================================================
   createServerSocket ( ) gets the server ready to handle  
   and manage multipe client terminals and messages 
   ========================================================= */
int createServerSocket(void)
{
    int listenFd;
    int allowReuse = 1;
    struct sockaddr_in serverAddr;

    // create listening TCP socket 
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // allows for reuse of socket port on restart 
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR,
                   &allowReuse, sizeof(allowReuse)) < 0)
    {
        perror("setsockopt");
        close(listenFd);
        exit(EXIT_FAILURE);
    }
    
    // resets and clears address values before initialization 
    memset(&serverAddr, 0, sizeof(serverAddr));

    // uses IPV4
    serverAddr.sin_family = AF_INET;

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    serverAddr.sin_port = htons(SERVER_PORT);

    // bind ( ) is used to connect socket to right port 
    if (bind(listenFd, (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        perror("bind");
        close(listenFd);
        exit(EXIT_FAILURE);
    }

    // listen ( ) is used to put socket into listening mode 
    if (listen(listenFd, BACKLOG) < 0)
    {
        perror("listen");
        close(listenFd);
        exit(EXIT_FAILURE);
    }

    return listenFd;
}

/* =========================================================
   handleClients( ) is the main server loop that controls
   the main processes to manage the multiple client server
   in these steps:
   
   1. Tracks connected clients in list
   2. listening socket listens for requests
   3. accepts messages and echoes back message 
   4. Remove and add clients to list 
   ========================================================= */
void handleClients(int listenFd)
{
    int clients[MAX_CLIENTS];
    fd_set readSet;
    int i;

    // initializes clients as empty 
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i] = -1;
    }

    // infinite while loop to run server 
    while (1)
    {
        int maxFd = listenFd;
        FD_ZERO(&readSet);

        // monitors listening socket for new requests 
        FD_SET(listenFd, &readSet);

        // add all active clients to the set 
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != -1)
            {
                FD_SET(clients[i], &readSet);

                if (clients[i] > maxFd)
                {
                    maxFd = clients[i];
                }
            }
        }


        if (select(maxFd + 1, &readSet, NULL, NULL, NULL) < 0)
        {
            perror("select");
            continue;
        }

        // Handles connection request 
        if (FD_ISSET(listenFd, &readSet))
        {
            int newSocket;
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);

            newSocket = accept(listenFd,
                               (struct sockaddr *)&clientAddr,
                               &clientLen);

            if (newSocket < 0)
            {
                perror("accept");
            }
            else
            {
                int clientIndex = addClient(clients, newSocket);

                if (clientIndex != -1)
             {
                printf("Client %d connected.\n", clientIndex + 1);
             }
            }
         }

        // monitor connected client sockets for messages 
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != -1 && FD_ISSET(clients[i], &readSet))
            {
                char message[BUFFER_SIZE];
                ssize_t bytesRead;

                // clears buffer 
                memset(message, 0, sizeof(message));

                // reads data using recv( ) 
                bytesRead = recv(clients[i], message,
                                 BUFFER_SIZE - 1, 0);

                if (bytesRead > 0)
                {
                
                    message[bytesRead] = '\0';

                    printf("Message received from client %d: %s", i + 1, message);

                    // echoes message back to client 
                    if (send(clients[i], message, bytesRead, 0) < 0)
                    {
                        perror("send");
                        removeClient(clients, i);
                    }
                }
                else if (bytesRead == 0)
                {
                    // recv() == 0 means the client closed the connection 
                    printf("Client in slot %d disconnected.\n", i);
                    removeClient(clients, i);
                }
                else
                {
                    // recv() < 0 means a socket read error happened
                    perror("recv");
                    removeClient(clients, i);
                }
            }
        }
    }
}

// addClient( ) stores client in array 
int addClient(int clients[], int newSocket)
{
    int i;

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == -1)
        {
            clients[i] = newSocket;
            return i;
        }
    }

    printf("Server full. Rejecting connection.\n");
    close(newSocket);
    return -1;
}

// removeClient() removes client from array 
void removeClient(int clients[], int index)
{
    if (clients[index] != -1)
    {
        close(clients[index]);
        clients[index] = -1;
    }
}

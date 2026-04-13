#include "server.h"

// runServer( ) builds the listening socket and hands control to 
// handleClients( ) function 
void runServer(void)
{
    int listenFd = createServerSocket();

    printf("Chat server is live on port %d.\n", SERVER_PORT);

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
    int isLoggedIn[MAX_CLIENTS];
    fd_set readSet;
    int i;

    // initializes clients as empty 
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i] = -1;
        isLoggedIn[i] = 0;
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
                isLoggedIn[clientIndex] = 0;
                printf("Client arrived and is waiting for login.\n");
              }
            }
         }

        // monitor connected client sockets for messages 
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != -1 && FD_ISSET(clients[i], &readSet))
       {
        char message[BUFFER_SIZE];
        char user[100];
        char pass[100];
        ssize_t bytesRead;

        memset(message, 0, sizeof(message));
        memset(user, 0, sizeof(user));
        memset(pass, 0, sizeof(pass));

       bytesRead = recv(clients[i], message, BUFFER_SIZE - 1, 0);

       if (bytesRead > 0)
        {
        message[bytesRead] = '\0';

        if (!isLoggedIn[i])
        {
            if (parseLoginMsg(message, user, pass) &&
                verifyUser(user, pass))
            {
                isLoggedIn[i] = 1;

                if (send(clients[i], "AUTH_OK", 7, 0) < 0)
                {
                    perror("Server could not return login approval");
                    removeClient(clients, isLoggedIn, i);
                }
                else
                {
                    printf("Client logged in as %s.\n", user);
                }
            }
            else
            {
                send(clients[i], "AUTH_FAIL", 9, 0);
                printf("Client failed login and was removed.\n");
                removeClient(clients, isLoggedIn, i);
            }
        }
        else
        {
            printf("Message from client: %s", message);

            if (send(clients[i], message, bytesRead, 0) < 0)
            {
                perror("Server could not echo message back");
                removeClient(clients, isLoggedIn, i);
            }
        }
    }
    else if (bytesRead == 0)
    {
        printf("Client closed the connection.\n");
        removeClient(clients, isLoggedIn, i);
    }
    else
    {
        perror("Server recv call failed");
        removeClient(clients, isLoggedIn, i);
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
void removeClient(int clients[], int isLoggedIn[], int index)
{
    if (clients[index] != -1)
    {
        close(clients[index]);
        clients[index] = -1;
        isLoggedIn[index] = 0;
    }
}

// helper function to parse the the message to match with text file 
int parseLoginMsg(const char *msg, char *user, char *pass)
{
    char localCopy[BUFFER_SIZE];
    char *token;

    if (strncmp(msg, "AUTH|", 5) != 0)
    {
        return 0;
    }

    memset(localCopy, 0, sizeof(localCopy));
    strncpy(localCopy, msg, sizeof(localCopy) - 1);

    token = strtok(localCopy, "|");
    if (token == NULL)
    {
        return 0;
    }

    token = strtok(NULL, "|");
    if (token == NULL)
    {
        return 0;
    }
    strncpy(user, token, 99);
    user[99] = '\0';

    token = strtok(NULL, "|");
    if (token == NULL)
    {
        return 0;
    }
    strncpy(pass, token, 99);
    pass[99] = '\0';

    return 1;
}

// helper function to help verify credentials 
int verifyUser(const char *user, const char *pass)
{
    FILE *file;
    char line[256];

    file = fopen(CREDENTIALS_FILE, "r");
    if (file == NULL)
    {
        perror("The credential file could not be opened");
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *fUser;
        char *fPass;

        line[strcspn(line, "\n")] = '\0';

        fUser = strtok(line, ",");
        fPass = strtok(NULL, ",");

        if (fUser != NULL && fPass != NULL)
        {
            if (strcmp(fUser, user) == 0 && strcmp(fPass, pass) == 0)
            {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

#include "client.h"

/* =========================================================
  runClient() is a main Level client funcion that: 
  
   1. connects to the server
   2. prompts user input
   3. enters the message transfer loop 
   4. closes the socket on exit
   ========================================================= */
void runClient(void)
{
    int serverFd = connectToServer();

    printf("Connected to the chat server.\n");

    if (!loginClient(serverFd))
    {
        close(serverFd);
        return;
    }

    printf("Access granted. You can start sending messages now.\n");
    printf("Enter any message and press Enter.\n");
    printf("Enter a negative number when you want to leave.\n");

    sendMessages(serverFd);
    close(serverFd);
}

/* =========================================================
  connectToServer creates a socket and connects to listening 
  socket. It uses functions socket() and connect(). 
   ========================================================= */
int connectToServer(void)
{
    int serverFd;
    struct sockaddr_in serverAddr;

    // create TCP socket 
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // resets and clears address structure values 
    memset(&serverAddr, 0, sizeof(serverAddr));

    // sets to IPV4 
    serverAddr.sin_family = AF_INET;

    // stores port value of socket for connection 
    serverAddr.sin_port = htons(SERVER_PORT);

    // converts IP address to binary 
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // connect () is called to establish connection 
    if (connect(serverFd, (struct sockaddr *)&serverAddr,
                sizeof(serverAddr)) < 0)
    {
        perror("connect");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    return serverFd;
}

// Function to authenticate user credentials during login 
int loginClient(int serverFd)
{
    char user[100];
    char pass[100];
    char loginMsg[BUFFER_SIZE];
    char serverReply[BUFFER_SIZE];
    ssize_t bytesRead;

    memset(user, 0, sizeof(user));
    memset(pass, 0, sizeof(pass));
    memset(loginMsg, 0, sizeof(loginMsg));
    memset(serverReply, 0, sizeof(serverReply));

    printf("Login username: ");
    if (fgets(user, sizeof(user), stdin) == NULL)
    {
        printf("Error. No username was entered.\n");
        return 0;
    }

    printf("Login password: ");
    if (fgets(pass, sizeof(pass), stdin) == NULL)
    {
        printf("Error. No password was entered.\n");
        return 0;
    }

    user[strcspn(user, "\n")] = '\0';
    pass[strcspn(pass, "\n")] = '\0';

    snprintf(loginMsg, sizeof(loginMsg), "AUTH|%s|%s", user, pass);

    if (send(serverFd, loginMsg, strlen(loginMsg), 0) < 0)
    {
        perror("Login request could not be sent");
        return 0;
    }

    bytesRead = recv(serverFd, serverReply, BUFFER_SIZE - 1, 0);

    if (bytesRead > 0)
    {
        serverReply[bytesRead] = '\0';

        if (strcmp(serverReply, "AUTH_OK") == 0)
        {
            return 1;
        }

        printf("Error. Credentials are invalid.\n");
        return 0;
    }
    else if (bytesRead == 0)
    {
        printf("Session closed before login finished.\n");
        return 0;
    }
    else
    {
        perror("Login reply was not received");
        return 0;
    }
}

/* =========================================================
   sendMessages( ) is used to facilitate message transfer 
   
   How input works:
   - fgets() reads input 
   - send() transmits that line to the server
   - recv() waits for the echoed reply
   - the echoed message is then printed to the terminal
   ========================================================= */
void sendMessages(int serverFd)
{
    char outgoing[BUFFER_SIZE];
    char incoming[BUFFER_SIZE];
    ssize_t bytesReceived;

    while (1)
    {
        // Clears buffers before next iteration 
        memset(outgoing, 0, sizeof(outgoing));
        memset(incoming, 0, sizeof(incoming));

        printf("> ");

        // Reads keyboard line using fgets( ) 
        if (fgets(outgoing, BUFFER_SIZE, stdin) == NULL)
        {
            printf("\nInput ended. Closing chat session.\n");
            break;
        }

        // User exit conditiion 
        {
         char *endPtr;
         long value = strtol(outgoing, &endPtr, 10);

         if (endPtr != outgoing && value < 0)
       {
           printf("Closing connection.\n");
           break;
       }
       }
  
        // send() to send message to server 
        if (send(serverFd, outgoing, strlen(outgoing), 0) < 0)
        {
            perror("Message could not be sent to the server.");
            break;
        }

      // waits for message to be echoed back 
        bytesReceived = recv(serverFd, incoming, BUFFER_SIZE - 1, 0);

        if (bytesReceived > 0)
        {
            incoming[bytesReceived] = '\0';
            printf("Echoed message: %s", incoming);
        }
        else if (bytesReceived == 0)
        {
            printf("Server closed the connection.\n");
            break;
        }
        else
        {
            perror("Message could not be recieved from the server.");
            break;
        }
    }
}

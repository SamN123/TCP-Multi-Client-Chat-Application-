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

    printf("Connected to server.\n");
    printf("Type any message and press Enter.\n");
    printf("Type negative number to exit.\n");

    sendMessages(serverFd);

    close(serverFd);
}

/* =========================================================
  connectToServer creates a socket and connects to lisening 
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
            printf("\nInput ended.\n");
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
            perror("send");
            break;
        }

        /* -------------------------------------------------
           Wait for the server to echo the message back:
           
           > 0 : reply received
           = 0 : server closed the connection
           < 0 : error
           ------------------------------------------------- */
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
            perror("recv");
            break;
        }
    }
}

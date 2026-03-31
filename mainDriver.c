#include "server.h"
#include "client.h"

/*Main driver function for server client echo app*/
int main(int argc, char *argv[])
{
    /* Choose mode between server or client*/
    if (argc != 2)
    {
        printf("Usage: %s server|client\n", argv[0]);
        return 1;
    }

    /* Start the server mode */
    if (strcmp(argv[1], "server") == 0)
    {
        run_server();
    }
    /* Start the client mode */
    else if (strcmp(argv[1], "client") == 0)
    {
        run_client();
    }
    else
    {
        printf("Invalid mode. Use: server or client\n");
        return 1;
    }

    return 0;
}

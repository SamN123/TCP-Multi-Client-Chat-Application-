#include "server.h"
#include "client.h"

// main driver to handle operations from all files.  
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s server|client\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "server") == 0)
    {
        runServer();
    }
    else if (strcmp(argv[1], "client") == 0)
    {
        runClient();
    }
    else
    {
        printf("Invalid mode. Use server or client.\n");
        return 1;
    }

    return 0;
}

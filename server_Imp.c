#include "server.h"

/* Server initialization */
void run_server(void)
{
    int listen_fd;

    listen_fd = create_server_socket();

    printf("Server started on port %d...\n", SERVER_PORT);

    run_server_loop(listen_fd);

    close(listen_fd);
}

/*Create and bind socket */
int create_server_socket(void)
{
    int listen_fd;
    int opt_value = 1;
    struct sockaddr_in server_addr;

    /* Create TCP socket */
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Allow quick restart after closing the server */
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value)) < 0)
    {
        perror("setsockopt");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    /* Fill in address information */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    /* Bind socket to port */
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    /* Start listening for incoming clients */
    if (listen(listen_fd, BACKLOG) < 0)
    {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    return listen_fd;
}

/* Main loop */
void run_server_loop(int listen_fd)
{
    int client_sockets[MAX_CLIENTS];
    fd_set read_set;
    int max_fd;
    int ready_count;
    int i;

    /* Mark all client slots as empty */
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = -1;
    }

    while (1)
    {
        /* Rebuild the read set on every loop */
        FD_ZERO(&read_set);
        FD_SET(listen_fd, &read_set);
        max_fd = listen_fd;

        /* Add active client sockets into the read set */
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] != -1)
            {
                FD_SET(client_sockets[i], &read_set);

                if (client_sockets[i] > max_fd)
                {
                    max_fd = client_sockets[i];
                }
            }
        }

        /* Wait for activity on any monitored socket */
        ready_count = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (ready_count < 0)
        {
            perror("select");
            continue;
        }

        /* If the listening socket is ready, accept a new client */
        if (FD_ISSET(listen_fd, &read_set))
        {
            int new_fd;
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);

            new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
            if (new_fd < 0)
            {
                perror("accept");
            }
            else
            {
                add_client(client_sockets, new_fd);

                printf("Client connected: %s:%d\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));
            }
        }

        /* Check each connected client for incoming data */
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] != -1 && FD_ISSET(client_sockets[i], &read_set))
            {
                char buffer[BUFFER_SIZE];
                ssize_t bytes_read;

                memset(buffer, 0, sizeof(buffer));

                /* Receive message from one client */
                bytes_read = recv(client_sockets[i], buffer, BUFFER_SIZE - 1, 0);

                if (bytes_read > 0)
                {
                    buffer[bytes_read] = '\0';

                    printf("Message received: %s", buffer);

                    /* Echo the same message back to the sender */
                    if (send(client_sockets[i], buffer, bytes_read, 0) < 0)
                    {
                        perror("send");
                        remove_client(client_sockets, i);
                    }
                }
                else if (bytes_read == 0)
                {
                    printf("Client in slot %d disconnected.\n", i);
                    remove_client(client_sockets, i);
                }
                else
                {
                    perror("recv");
                    remove_client(client_sockets, i);
                }
            }
        }
    }
}

/* Added client */
void add_client(int client_sockets[], int new_fd)
{
    int i;

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == -1)
        {
            client_sockets[i] = new_fd;
            return;
        }
    }

    printf("Server full. Connection rejected.\n");
    close(new_fd);
}

/*Remove client */
void remove_client(int client_sockets[], int index)
{
    if (client_sockets[index] != -1)
    {
        close(client_sockets[index]);
        client_sockets[index] = -1;
    }
}

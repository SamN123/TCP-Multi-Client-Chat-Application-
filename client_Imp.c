#include "client.h"

/* Run cient function */
void run_client(void)
{
    int server_fd;

    server_fd = connect_to_server();

    printf("Connected to server.\n");
    printf("Type a message and press Enter.\n");
    printf("Type /quit to exit.\n");

    run_client_loop(server_fd);

    close(server_fd);
}

/* Function to create socket and connect to server */
int connect_to_server(void)
{
    int server_fd;
    struct sockaddr_in server_addr;

    /* Create TCP socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Fill in server address information */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Connect to the server */
    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

/*Message exchange server loop*/
void run_client_loop(int server_fd)
{
    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while (1)
    {
        memset(send_buffer, 0, sizeof(send_buffer));
        memset(recv_buffer, 0, sizeof(recv_buffer));

        printf("> ");

        /* Read one line */
        if (fgets(send_buffer, BUFFER_SIZE, stdin) == NULL)
        {
            printf("\nInput ended.\n");
            break;
        }

        /* client acknowledges and exits */
        if (strncmp(send_buffer, "/quit", 5) == 0)
        {
            printf("Closing connection.\n");
            break;
        }

        /* Send typed message to server */
        if (send(server_fd, send_buffer, strlen(send_buffer), 0) < 0)
        {
            perror("send");
            break;
        }

        /* Receive echoed reply from server */
        bytes_received = recv(server_fd, recv_buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0)
        {
            recv_buffer[bytes_received] = '\0';
            printf("Echoed back: %s", recv_buffer);
        }
        else if (bytes_received == 0)
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

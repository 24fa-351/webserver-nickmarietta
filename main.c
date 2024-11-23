#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "http_message.h"
#include "request.h"

#define DEFAULT_PORT 8080
#define LISTEN_BACKLOG 50

int main(int argc, char *argv[])
{
    int some_port = DEFAULT_PORT;

    // specify the port number
    if (argc == 3)
    {
        if (strcmp(argv[1], "-p") == 0)
        {
            some_port = atoi(argv[2]);
        }
        printf("Listening to port number: %d\n", some_port);
    }

    // create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(some_port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind the socket to error code
    int error_code;
    error_code = bind(server_socket, (struct sockaddr *)&socket_address, sizeof(socket_address));

    if (error_code == -1)
    {
        printf("Failed to bind the socket\n");
        return 1;
    }

    // listen for incoming connections
    error_code = listen(server_socket, LISTEN_BACKLOG);

    if (error_code == -1)
    {
        printf("Failed to listen for incoming connections\n");
        return 1;
    }

    // accept the incoming connection
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    while (1)
    {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == -1)
        {
            printf("Failed to accept the connection\n");
            return 1;
        }

        // handle the connection
        handleConnection(client_socket);
        printf("Connection handled\n");

        // get pthread id
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        // time for them threads
        pthread_t thread_id;
        int thread_create_result = pthread_create(&thread_id, NULL, handleConnection, client_socket_ptr);

        if (thread_create_result != 0)
        {
            printf("Failed to create a thread\n");
            close(client_socket);
            free(client_socket_ptr);
            return 1;
        }
    }
    // close the connection and return 0
    close(server_socket);
    return 0;
}
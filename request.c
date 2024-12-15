#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "request.h"

static size_t requests_total = 0;
static size_t bytes_received = 0;
static size_t bytes_sent = 0;

// specify name of a file in "/static" directory
int static_endpoint(int client_socket, char *path) {
    return 1;
};

// returns properly formatted HTML doc that lists num. of requests
int stats_endpoint(int client_socket, char *path, int request_count, int bytes_received, int bytes_sent)
{
    char body[1024];

    int body_length = snprintf(body, sizeof(body),
                               "<html><body><h1>Stats</h1>"
                               "<p>Request count: %d</p>"
                               "<p>Sent bytes: %d</p>"
                               "<p>Received bytes: %d</p></body></html>\n",
                               request_count, bytes_sent, bytes_received);

    char response[1024];
    int response_length = snprintf(response, sizeof(response),
                                   "HTTP/1.1 200 OK\n"
                                   "Content-Type: text/html\n"
                                   "Content-Length: %d\n"
                                   "\n"
                                   "%s",
                                   body_length, body);

    if (write(client_socket, response, response_length) == -1) // error handling
    {
        perror("Failed to write to client");
    }
    return response_length;
};

// return text or HTML, summing value of 2 query params (a and b) BOTH NUMERIC!
int calc_endpoint(int client_socket, char *path)
{
    int a;
    int b;
    sscanf(path, "/calc?a=%d&b=%d", &a, &b);

    char response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>%d + %d = "
                    "%d</h1></body></html>";
    bytes_received += (sizeof(response) - 1);

    // do the calculation
    dprintf(client_socket, response, a, b, a + b);
};

void handlePaths(int client_socket, char *buffer)
{
    char method[16];
    char path[1024];
    char http_version[16];

    sscanf(buffer, "%s %s %s", method, path, http_version);

    if (strncmp(path, "/static/", 8) == 0)
    {
        static_endpoint(client_socket, path);
    }
    else if (strncmp(path, "/stats", 6) == 0)
    {
        stats_endpoint(client_socket, path, requests_total, bytes_received, bytes_sent);
    }
    else if (strncmp(path, "/calc", 5) == 0)
    {
        calc_endpoint(client_socket, path);
    }
    else
    {
        char error_response[1024];
        int error_response_length = snprintf(error_response, sizeof(error_response),
                                             "<html><body><h1>404 Not Found</h1></body></html>\n");
        write(client_socket, error_response, error_response_length);
    }
};

void handleConnection(int *someClient)
{
    int socket_fd = *((int *)someClient);
    free(someClient);

    printf("Connection received and handling on %d\n", socket_fd);

    // keep reading until closed
    while (1)
    {
        char buffer[1024];
        int read_bytes = read(socket_fd, buffer, sizeof(buffer));

        if (read_bytes == -1)
        {
            perror("Failed to read from client\n");
            break;
        }
        if (read_bytes == 0)
        {
            printf("Connection closed by client\n");
            break;
        }
        if (read_bytes > 0)
        {
            buffer[read_bytes] = '\0';
            printf("Received: %s from connection %d\n", buffer, socket_fd);
        }
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Closing connection\n");
            break;
        }
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Closing connection\n");
            break;
        }

        requests_total++;
        bytes_received += read_bytes;
        handlePaths(socket_fd, buffer);
    }
    printf("Closing connection on %d\n", socket_fd);
    close(socket_fd);
    return NULL;
};
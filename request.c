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
int static_endpoint(int client_socket, char *path)
{
    char path_to_file[1024];
    snprintf(path_to_file, sizeof(path_to_file), "static%s", path);
    path = path_to_file;

    struct stat file_stat;
    if (stat(path, &file_stat) == -1)
    {
        char error_response[1024];
        int error_response_length = snprintf(error_response, sizeof(error_response),
                                            "<html><body><h1>404 Not Found</h1></body></html>\n");
        write(client_socket, error_response, error_response_length);
        return -1;
    }
    // used from struct
    int file_size = file_stat.st_size;
    int file_fd = open(path, O_RDONLY);

    dprintf(client_socket,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: image/png\r\n"
            "Content-Disposition: inline; filename=\"%s\"\r\n"
            "Content-Length: %d\r\n"
            "\r\n",
            path + 1, file_size);
    
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        write(client_socket, buffer, bytes_read);
    }
    close(file_fd);
};

// returns properly formatted HTML doc that lists num. of requests
int stats_endpoint(int client_socket) {
    char response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Number of request: "
        "%d</h1><h1>Total Bytes Received: %d</h1><h1>Total Bytes Sent:%d</h1></body></html>";
    bytes_sent += (sizeof(response) - 1);
    dprintf(client_socket, response, requests_total, bytes_received, bytes_sent);
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

void handlePaths(int client_socket, char buffer)
{
    char method[16];
    char path[1024];
    char http_version[16];

    sscanf(buffer, "%s %s %s", method, path, http_version);

    if (strncmp(path, "/static/", 8) == 0)
    {
        static_endpoint(client_socket, path);
    }
    else if (strncmp(path, "/stats", 7) == 0)
    {
        stats_endpoint(client_socket);
    }
    else if (strncmp(path, "/calc", 6) == 0)
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
    int socket_fd = *someClient;
    free(someClient);

    printf("Connection received and handling on %d\n", socket_fd);

    // keep reading until closed
    while (1)
    {
        char buffer[BUFFER_SIZE];
        int read_bytes = read(socket_fd, buffer, sizeof(buffer));

        if (read_bytes == -1)
        {
            printf("Failed to read from client\n");
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
};

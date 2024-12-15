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
#include "http_message.h"

static size_t requests_total = 0;
static size_t bytes_received = 0;
static size_t bytes_sent = 0;

// specify name of a file in "/static" directory
int static_endpoint(int client_socket, char *path)
{
    char file[1024];
    if (sscanf(path, "/static/%s", file) == 1)
    {
        int fd = open(file, O_RDONLY);
        if (fd == -1)
        {
            struct stat st;
            fstat(fd, &st);
            char *file_contents = malloc(st.st_size);
            read(fd, file_contents, st.st_size);
            close(fd);

            char response[1024];
            int response_length = snprintf(response_length, sizeof(response_length),
                                           "HTTP/1.1 200 OK\r\n"
                                           "Content-Length: %ld\r\n"
                                           "Content-Type: image/jpeg\r\n"
                                           "Content-Disposition: inline\r\n"
                                           "\r\n",
                                           (long)st.st_size);

            write(client_socket, response, response_length);
            write(client_socket, file_contents, st.st_size);

            free(file_contents);
            return (response_length + st.st_size);
        }
        else
        {
            char error_response[1024];
            int error_response_length = snprintf(error_response, sizeof(error_response),
                                                 "HTTP/1.1 404 Not Found\r\n"
                                                 "Content-Length: 0\r\n"
                                                 "Content-Type: text/html\r\n"
                                                 "\r\n");

            write(client_socket, error_response, error_response_length);
            return 0;
        }
        return 0;
    }
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

    if (write(client_socket, response, response_length) == -1 || write(client_socket, response, response_length) != response_length) // error handling
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

void *handleConnection(int *someClient)
{
    fprintf(stderr, "handling connection\n");
    int client_socket = *((int *)someClient);
    free(someClient);

    while (1)
    {
        http_client_message_t *http_msg = NULL;
        http_read_result_t result;

        read_http_client_message(client_socket, &http_msg, &result,
                                 &bytes_received);
        if (result == CLOSED_CONNECTION)
        {
            fprintf(stderr, "Connection closed\n");
            close(client_socket);
            return NULL;
        }
        if (result == BAD_REQUEST)
        {
            fprintf(stderr, "Bad request\n");
            close(client_socket);
            return NULL;
        }
        if (result == MESSAGE)
        {
            fprintf(stderr, "Message received\n");
            fprintf(stderr,
                    "Received HTTP message: method=%s, path=%s, http_version=%s, "
                    "body=%s\n",
                    http_msg->method, http_msg->path, http_msg->http_version,
                    http_msg->body ? http_msg->body : "NULL");

            handlePaths(client_socket, http_msg);
        }

        if (http_msg)
        {
            free_http_client_message(http_msg);
        }
    }
    return NULL;
}
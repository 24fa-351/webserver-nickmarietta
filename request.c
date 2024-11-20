#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "request.h"

// specify name of a file in "/static" directory
int static_endpoint(int client_socket, char *path) {
    // something;
};

// returns properly formatted HTML doc that lists num. of requests
int stats_endpoint(int client_socket, char *path, int request_amt, int sent_bytes, int received_bytes) {

};

// return text or HTML, summing value of 2 query params (a and b) BOTH NUMERIC!
int calc_endpoint(int client_socket, char *path)
{
    int a;
    int b;
    if (sscanf(path, "/calc?a=%d&b=%d", &a, &b) == 2)
    {
        char response[1024];
        int response_length = sprintf(response, sizeof(response),
                                      "HTTP/1.1 200 OK\r\n"
                                      "Content-Length: %d\r\n"
                                      "Content-Type: text/html\r\n"
                                      "\r\n"
                                      "<html><body><h1>Sum: %d</h1></body></html>\n",
                                      39 + snprintf(NULL, 0, "%d", a + b), a + b);

        if (write(client_socket, response, response_length) == -1)
        {
            printf("Failed to write to client\n");
            return 500;
        }
        return response_length;
    }
    // if sscanf failed
    else
    {
        char error_response[1024];
        int error_response_length = sprintf(error_response, sizeof(error_response),
                                            "<html><body><h1>404 Not Found</h1></body></html>\n");
        write(client_socket, error_response, error_response_length);
        return 0;
    }
};
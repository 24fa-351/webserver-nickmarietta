#ifndef REQUEST_H
#define REQUEST_H

#define BUFFER_SIZE 1024

// specify name of a file in "/static" directory
int static_endpoint(int client_socket, char *path);

// returns properly formatted HTML doc that lists num. of requests
int stats_endpoint(int client_socket);

// return text or HTML, summing value of 2 query params (a and b) BOTH NUMERIC!
int calc_endpoint(int client_socket, char *path);

// to handle different paths
void handlePaths(int client_socket, char buffer);

// from echo_server
void handleConnection(int *someClient);

#endif
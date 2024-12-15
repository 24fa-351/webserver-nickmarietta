#ifndef REQUEST_H
#define REQUEST_H

// specify name of a file in "/static" directory
int static_endpoint(int client_socket, char *path);

// returns properly formatted HTML doc that lists num. of requests
int stats_endpoint(int client_socket, char *path, int request_count, int bytes_received, int bytes_sent);

// return text or HTML, summing value of 2 query params (a and b) BOTH NUMERIC!
int calc_endpoint(int client_socket, char *path);

// to handle different paths
void handlePaths(int client_socket, char *buffer);

// from echo_server
void *handleConnection(int *someClient);

#endif
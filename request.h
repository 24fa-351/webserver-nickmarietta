#ifndef REQUEST_H
#define REQUEST_H

// specify name of a file in "/static" directory
int static_endpoint(int client_socket, char *path);

// returns properly formatted HTML doc that lists num. of requests
int stats_endpoint(int client_socket, char *path, int request_amt, int sent_bytes, int received_bytes);

// return text or HTML, summing value of 2 query params (a and b) BOTH NUMERIC!
int calc_endpoint(int client_socket, char *path);

#endif
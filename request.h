#ifndef REQUEST_H
#define REQUEST_H

typedef struct {
    char* method;
    char* path;
    char* http_version;
} Request;

Request* request_read_from_fd(int fd);

void request_print(Request* request);

void request_free(Request* request);

#endif
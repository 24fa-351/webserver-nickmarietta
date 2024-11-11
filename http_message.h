#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

typedef struct msg {
  char *method;
  char *path;
  char *http_version;
  char *headers;
  char *body;
  int body_length;
} http_client_message_t;

typedef enum { BAD_REQUEST, CLOSED_CONNECTION, OK } http_read_result_t;

bool is_complete_http_message(char *buffer);

void read_http_client_message(int client_socket, http_client_message_t *message, http_read_result_t *result);

void free_http_client_message(http_client_message_t *message);

#endif
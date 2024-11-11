#include "http_message.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool is_complete_http_message(char *buffer) {
    // find out what the HTTP message looks like (starts with GET and ends with blank line)
    if (strlen(buffer) < 10) {
        return false;
    }
    if (strncmp(buffer, "GET", 4) != 0) {
        return false;
    }
    if (strncmp(buffer + strlen(buffer) - 4, "\r\n\r\n", 4) != 0) {
        return false;
    }
}

void read_http_client_message(int client_socket, http_client_message_t *message,
                              http_read_result_t *result) {
  printf("Reading the message from client...\n");
  *message = malloc(sizeof(http_client_message_t));
  *result = CLOSED_CONNECTION;
  if (message == NULL) {
    printf("Failed to allocate memory for the message\n");
    result = NULL;
    return;
  }

  char buffer[4096];
  strcpy(buffer, "");


}

void free_http_client_message(http_client_message_t *message) {
  printf("Freeing the message...\n");
  free(message->method);
  free(message->path);
  free(message->http_version);
  free(message->headers);
  free(message->body);
  free(message);
}
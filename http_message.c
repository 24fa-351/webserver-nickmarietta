#include "http_message.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

bool is_complete_http_message(char *buffer)
{
  if (strlen(buffer) < 10)
  {
    return false;
  }
  if (strstr(buffer, "\r\n\r\n") != NULL)
  {
    return false;
  }
  if (strncmp(buffer, "GET", 3) != 0)
  {
    return false;
  }
  return true;
};

void read_http_client_message(int client_socket, http_client_message_t **message,
                              http_read_result_t *result)
{
  char buffer[1024];
  int bytes_read = read(client_socket, buffer, sizeof(buffer));
  if (bytes_read == -1)
  {
    printf("Failed to read from client\n");
    *result = BAD_REQUEST;
    return;
  }
  if (bytes_read == 0)
  {
    *result = CLOSED_CONNECTION;
    return;
  }
  if (!is_complete_http_message(buffer))
  {
    *result = BAD_REQUEST;
    return;
  }

  *result = OK;
  sscanf(buffer, "%ms %ms %ms", &(*message)->method, &(*message)->path, &(*message)->http_version);

  // print the message
  printf("Method: %s\n", (*message)->method);
  printf("Path: %s\n", (*message)->path);
  printf("HTTP Version: %s\n", (*message)->http_version);
  printf("Headers: %s\n", (*message)->headers);
  printf("Body: %s\n", (*message)->body);
  printf("Body Length: %d\n", (*message)->body_length);

};

void free_http_client_message(http_client_message_t *message)
{
  printf("Freeing the message...\n");
  free(message->method);
  free(message->path);
  free(message->http_version);
  free(message->headers);
  free(message->body);
  free(message);
}
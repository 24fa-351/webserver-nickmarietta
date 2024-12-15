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
  // check to see if message is complete (web browser)
  if (strstr(buffer, "\r\n\r\n") != NULL || strstr(buffer, "\n\n") != NULL)
  {
    return true;
  }
  if (strncmp(buffer, "GET", 4) != 0)
  {
    return false;
  }
  return false;
};

void read_http_client_message(int client_socket, http_client_message_t **message,
                              http_read_result_t *result, int *bytes_read)
{
  fprintf(stderr, "Reading the message...\n"); // debug
  *message = malloc(sizeof(http_client_message_t));
  char buffer[1024];
  strcpy(buffer, "");

  while (!is_complete_http_message(buffer))
  {
    int read_bytes = read(client_socket, buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1);

    if (read_bytes == 0)
    {
      *result = CLOSED_CONNECTION;
      return;
    }
    if (read_bytes == -1 || read_bytes == 0)
    {
      *result = BAD_REQUEST;
      return;
    }

    // so buffer is null-terminated
    buffer[read_bytes + strlen(buffer)] = '\0';
  }
  *bytes_read = strlen(buffer);
  char *method = strtok(buffer, " ");
  char *path = strtok(NULL, " ");
  char *http_version = strtok(NULL, "\r\n");

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
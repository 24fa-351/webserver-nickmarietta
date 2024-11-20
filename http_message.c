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
}

void read_http_client_message(int client_socket, http_client_message_t **message,
                              http_read_result_t *result, int *bytes_received)
{
  printf("Reading the message from client...\n");
  *message = malloc(sizeof(http_client_message_t));
  *result = CLOSED_CONNECTION;
  if (message == NULL)
  {
    printf("Failed to allocate memory for the message\n");
    result = NULL;
    return;
  }

  char buffer[4096];
  strcpy(buffer, "");

  while (!is_complete_http_message(buffer))
  {
    int bytes_read = read(client_socket, buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1);
    if (bytes_read == 0)
    {
      printf("Connection closed by client\n");
      *result = CLOSED_CONNECTION;
      return;
    }
    if (bytes_read == -1)
    {
      printf("Failed to read from client\n");
      *result = BAD_REQUEST;
      return;
    }
    // make sure the buffer is null-terminated
    buffer[bytes_read] = '\0';
  }
  // time to parse the message
  *bytes_received += strlen(buffer);
  char *method = strtok(buffer, " ");
  char *path = strtok(NULL, " ");
  char *http_version = strtok(NULL, "\r\n");

  if (method == NULL || path == NULL || http_version == NULL)
  {
    printf("Failed to parse the message\n");
    *result = BAD_REQUEST;
    return;
  }
  // copy the values to the message
  (*message)->method = strdup(method);
  (*message)->path = strdup(path);
  (*message)->http_version = strdup(http_version);

  if (strcmp((*message)->method, "GET") != 0)
  {
    printf("Unsupported method: %s\n", (*message)->method);
    *result = BAD_REQUEST;
    return;
  }

  // find the headers from here
  char *header = strstr(buffer, "\r\n\r\n");
  if (header)
  {
    (*message)->headers = strdup(header);
  }
  else
  {
    (*message)->headers = NULL;
  }

  // go through the body from here
  if (header == '\0') {
    (*message)->body = NULL;
    (*message)->body_length = 0;
  } else {
    (*message)->body = strdup(header);
    (*message)->body_length = strlen(header);
  }

  // declare result
}



  void free_http_client_message(http_client_message_t * message)
  {
    printf("Freeing the message...\n");
    free(message->method);
    free(message->path);
    free(message->http_version);
    free(message->headers);
    free(message->body);
    free(message);
  }
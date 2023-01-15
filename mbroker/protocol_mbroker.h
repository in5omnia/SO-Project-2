#ifndef PROJETO_SO_2_PROTOCOL_MBROKER_H
#define PROJETO_SO_2_PROTOCOL_MBROKER_H

#include "../protocol/codes.h"
#include "../protocol/default_sizes.h"

typedef struct __attribute__((__packed__)) request {
  code_t code;
  client_pipe_path_t client_named_pipe_path;
  union {
    box_name_t box_name;
  };
} request_t;

/* Reads Request from FIFO and returns the request
   fifo:  FIFO file descriptor
   buffer: request buffer
   */
void parse_requests(int fifo, request_t *buffer);

#endif // PROJETO_SO_2_PROTOCOL_MBROKER_H

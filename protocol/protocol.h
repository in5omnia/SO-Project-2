//
// Created by Beatriz Gavilan on 13/01/2023.
//
#include "codes.h"
#include "default_sizes.h"
#include "string.h"

#ifndef PROJETO_SO_2_PROTOCOL_H
#define PROJETO_SO_2_PROTOCOL_H

typedef struct __attribute__((__packed__)) client_request {
	code_t code;
	client_pipe_path_t client_named_pipe_path;

	box_name_t box_name;

} client_request_t;

typedef struct __attribute__((__packed__)) message {
	code_t code;
	char message[MAX_MESSAGE_SIZE];

} message_t;

//sends
int send_request_to_server(char *fifo_name, client_request_t* request);

client_request_t *create_client_request(code_t code,
										char *client_named_pipe_path,
										char *box_name);

#endif //PROJETO_SO_2_PROTOCOL_H

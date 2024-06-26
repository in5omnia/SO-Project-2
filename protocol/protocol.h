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



typedef struct __attribute__((__packed__)) create_remove_box_request_t {
	code_t code;
	client_pipe_path_t client_named_pipe_path;
	box_name_t box_name;
} create_remove_box_request_t;

typedef struct __attribute__((__packed__)) list_boxes_request_t {
	code_t code;
	client_pipe_path_t client_named_pipe_path;
} list_boxes_request_t;

typedef struct __attribute__((__packed__)) create_remove_box_response_t { // INTERNAL THIS SOULD BE AVAILABLE TO ALL
	code_t code;
	ret_code_t ret_code;
	error_message_t error_message;
} create_remove_box_response_t;

typedef struct __attribute__((__packed__)) list_boxes_responses_t {
	code_t code;
	last_t last;
	box_name_t box_name;
	box_size_t box_size;
	n_publishers_t n_publishers;
	n_subscribers_t n_subscribers;
} list_boxes_response_t;

create_remove_box_request_t
build_create_remove_box_request(code_t code, client_pipe_path_t client_named_pipe_path, box_name_t box_name);

create_remove_box_response_t
build_create_remove_box_response(code_t code, ret_code_t ret_code, error_message_t error_message);


list_boxes_request_t build_list_boxes_request(client_pipe_path_t client_named_pipe_path);

list_boxes_response_t
build_list_boxes_response(code_t code, last_t last, box_name_t box_name, box_size_t box_size, n_publishers_t n_pub,
						  n_subscribers_t n_sub);




//sends
int send_request_to_server(char *fifo_name, client_request_t* request);

client_request_t *create_client_request(code_t code,
										char *client_named_pipe_path,
										char *box_name);

#endif //PROJETO_SO_2_PROTOCOL_H

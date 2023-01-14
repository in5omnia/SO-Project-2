//
// Created by Beatriz Gavilan on 13/01/2023.
//
#import "codes.h"
#import "../mbroker/default_sizes.h"
#ifndef PROJETO_SO_2_PROTOCOL_H
#define PROJETO_SO_2_PROTOCOL_H

typedef struct __attribute__((__packed__)) request {
	code_t code;
	client_pipe_path_t client_named_pipe_path;
	union{
		box_name_t box_name;
	};
} request_t;

#endif //PROJETO_SO_2_PROTOCOL_H

//
// Created by Eduardo Nazário on 13/01/2023.
//
#include <stdint.h>

#ifndef PROJETO_SO_2_DEFAULT_SIZES_H
#define PROJETO_SO_2_DEFAULT_SIZES_H

#define LAST 1
#define NOT_LAST 0

#define END_MESSAGE '\0'

#define MAX_CLIENTPIPE_NAME 256
#define MAX_BOX_NAME 32

#define MAX_MESSAGE_SIZE 1024
#define MAX_ERRORMESSAGE_SIZE 1024

typedef uint8_t code_t;
typedef int32_t ret_code_t;
typedef uint8_t last_t;
typedef uint64_t box_size_t;
typedef uint64_t n_publishers_t;
typedef uint64_t n_subscribers_t;
typedef char error_message_t[MAX_ERRORMESSAGE_SIZE];
typedef char client_pipe_path_t[MAX_CLIENTPIPE_NAME];
typedef char box_name_t[MAX_BOX_NAME];


#endif //PROJETO_SO_2_DEFAULT_SIZES_H

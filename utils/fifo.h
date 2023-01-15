//
// Created by Beatriz Gavilan on 14/01/2023.
//

#ifndef PROJETO_SO_2_FIFO_H
#define PROJETO_SO_2_FIFO_H

#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "logging.h"

int create_fifo(char *pipe_name);

int start_fifo(char *pipe_name, int flag);

#endif //PROJETO_SO_2_FIFO_H

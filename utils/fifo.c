//
// Created by Beatriz Gavilan on 14/01/2023.
//

#include "fifo.h"

int create_fifo(char *pipe_name) {
	if (mkfifo(pipe_name, 0666) == -1) { // FIXME 0666 INTERNAL
		PANIC("Failed to create fifo, Errno: %d", errno);
	}
	INFO("Fifo created: %s", pipe_name);
	return 0;
}

int start_fifo(char *pipe_name, int flag) {
	int fifo = open(pipe_name, flag); // FIXME bc RDWR - n sei se tem de ser dif instancias
	if (fifo == -1)
		PANIC("Failed to open fifo, Errno: %d",
			  errno); // TODO: add error handling/Reason

	INFO("FIFO opened in %d: %s", flag, pipe_name);

	return fifo;
}
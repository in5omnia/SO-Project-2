#include "protocol.h"
#include "logging.h"
#include <unistd.h>
#include <sys/fcntl.h>

int send_request_to_server(char *fifo_name, client_request_t *request) {
	int fifo = open(fifo_name, O_WRONLY);
	if (fifo == -1) {
		PANIC("Failed to open fifo");
		return -1;
	}
	ssize_t bytes_written = write(fifo, request, sizeof(client_request_t));
	if (bytes_written == -1) {
		PANIC("Failed to write to FIFO");
		return -1;
	}
	close(fifo);
	return 0;
}

client_request_t *create_client_request(code_t code,
										char *client_named_pipe_path,
										char *box_name) {
	client_request_t *request = (client_request_t *) malloc(sizeof(client_request_t));
	if (request == NULL) {
		PANIC("Failed to allocate memory");
		return NULL;
	}

	request->code = code;
	memcpy(request->client_named_pipe_path, client_named_pipe_path, MAX_CLIENTPIPE_NAME);
	memcpy(request->box_name, box_name, MAX_BOX_NAME);

	return request;
}
#include "logging.h"

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
 // fprintf(stderr, "usage: sub <register_pipe_name> <pipe_name> <box_name>\n");
  //WARN("unimplemented"); // TODO: implement
	signal(SIGINT, exit_process);
	signal(SIGPIPE, exit_process);

	// TODO: use args
	char buffer[MAX_INPUT_SIZE];
	char box_name[MAX_BOX_NAME];

	int s = read_input(buffer, client_pipe_name, box_name);
	while (s == -1) {
		fprintf(stderr, "usage: sub <register_pipe_name> <client_pipe_name> <box_name>\n");
		INFO("Reading input");
		s = read_input(buffer, client_pipe_name, box_name);
	}
	char *register_pipe_name = strdup(buffer);
	if (register_pipe_name == NULL) {
		PANIC("Failed to allocate memory");
	}
	INFO("Input read");

	create_fifo(client_pipe_name);

	client_request_t *request = create_client_request(CODE_REGISTER_SUBSCRIBER, client_pipe_name, box_name);
	if (send_request_to_server(register_pipe_name, request) != 0) {
		//FIXME should i delete fifo?
		PANIC("Failed to send request to server");
	}
	free(register_pipe_name);
	client_pipe = start_fifo(client_pipe_name, O_RDONLY);//FIXME check if it worked?
	//threads sending messages?
	while (1) {
		receive_messages(client_pipe);
	}

	return 0;
}

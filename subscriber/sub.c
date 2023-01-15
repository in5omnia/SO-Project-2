#include "logging.h"
#include "../protocol/codes.h"
#include "../protocol/default_sizes.h"
#include "../protocol/protocol.h"
#include <sys/fcntl.h>
#include "fifo.h"
#include <signal.h>

char client_pipe_name[MAX_CLIENTPIPE_NAME];
int num_messages = 0;
int client_pipe;

void exit_process(int sig){
	INFO("Exiting subscriber process %d", sig);
	size_t bytes = fwrite(&num_messages, sizeof(int), 1, stdout);
	if (bytes != MAX_MESSAGE_SIZE) {
		PANIC("Failed to write to stdout");
	}
	close(client_pipe);
	unlink(client_pipe_name);
	exit(0);	//sessions--
}


void print_message(char* message){
	int bytes = fprintf(stdout, "%s\n", message);
	if (bytes != MAX_MESSAGE_SIZE) {	// FIXME: or message len?
		PANIC("Failed to write to stdout");
	}
}

int receive_messages(int fifo){
	message_t* message = (message_t*) malloc(sizeof(message_t));
	ssize_t bytes_read = read(fifo, message, sizeof(message_t));
	if (bytes_read == -1) {
		PANIC("Failed to read from FIFO");
		return -1;
	}
	if (bytes_read == 0){	//end of file
		// writin end of pipe has been closed - subscriber process must end
		exit_process(0);
		return -1;
	}
	else {
		num_messages++;
		print_message(message->message);
	}
	free(message);
	return 0;
}



int read_input(char *buffer, char *client_pipe_namepath, char *box_name) {
	char input[MAX_INPUT_SIZE];
	char assert[MAX_INPUT_SIZE];
	if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
		PANIC("Failed to read from stdin");
		return -1;
	}

	ssize_t scan_matches = sscanf(input, "sub %s %s %s %s", buffer, client_pipe_namepath, box_name, assert);
	if (scan_matches != 3) {
		return -1;
	}
	size_t client_pipe_len = strlen(client_pipe_namepath);
	size_t box_name_len = strlen(box_name);
	memset(client_pipe_namepath + client_pipe_len, '\0', MAX_CLIENTPIPE_NAME - client_pipe_len);    //FIXME check for error
	memset(box_name + box_name_len, '\0', MAX_BOX_NAME - box_name_len);    //FIXME check for error
	// fills the names name with '\0'
	return 0;
}

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

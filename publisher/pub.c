#include "../protocol/codes.h"
#include "../protocol/default_sizes.h"
#include "../protocol/protocol.h"
#include "logging.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/fcntl.h>

#include "fifo.h"


int client_pipe;
char client_pipe_name[MAX_CLIENTPIPE_NAME];

void exit_process(){
	close(client_pipe);
	unlink(client_pipe_name);
	exit(0);
}

char *get_message() {
	char *message = (char*)malloc(sizeof(char)*MAX_MESSAGE_SIZE);
	//FIXME vai dar erro se a message for grande demais - tenho de a truncar e por o char 256 a \0 INTERNAL
	ssize_t bytes_read = fread(message, sizeof(char), MAX_MESSAGE_SIZE, stdin);

	if (bytes_read == MAX_MESSAGE_SIZE) {
		if (feof(stdin)) {
			exit_process();
		}
		PANIC("Failed to read from stdin");
		return NULL;
	}


	int len = strlen(message);
	if (message[len-1] == '\n') {	//replace newline
		message[len-1] = '\0';
	}
	if (strlen(message) < MAX_MESSAGE_SIZE) {
		memset(message + len, '\0', MAX_MESSAGE_SIZE - len);	//FIXME check for error
		// fills the rest of the message with '\0'§
	}
	return message;
}


int publish_message(int client_pipe, char *message) {
	message_t *msg = (message_t*)malloc(sizeof(message_t));
	msg->code = CODE_PUBLISHER_SEND_MSG;
	memcpy(msg->message, message, MAX_MESSAGE_SIZE);
	ssize_t byteswritten = write(client_pipe, msg, MAX_MESSAGE_SIZE);
	if (byteswritten == -1) {
		free(msg);
		PANIC("Failed to write to FIFO");
		return -1;
	}
	free(msg);
	return 0;
}


int read_input(char *buffer, char *client_pipe_name, char *box_name) {
	char input[MAX_INPUT_SIZE];
	char assert[MAX_INPUT_SIZE];
	if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
		if (feof(stdin)){
			exit_process();
		}
		PANIC("Failed to read from stdin");
		return -1;
	}

	ssize_t scan_matches = sscanf(input, "pub %s %s %s %s", buffer, client_pipe_name, box_name, assert);
	if (scan_matches != 3) {
		return -1;
	}
	int client_pipe_len = strlen(client_pipe_name);
	int box_name_len = strlen(box_name);
	memset(client_pipe_name + client_pipe_len, '\0', MAX_CLIENTPIPE_NAME - client_pipe_len);    //FIXME check for error
	memset(box_name + box_name_len, '\0', MAX_BOX_NAME - box_name_len);    //FIXME check for error
	// fills the names name with '\0'
	return 0;
}


int main(int argc, char **argv) {
	(void) argc;
	(void) argv;


	// TODO: use args
	char buffer[MAX_INPUT_SIZE];
	char box_name[MAX_BOX_NAME];

	INFO("Reading input");
	int s = read_input(buffer, client_pipe_name, box_name);
	while (s == -1) {
		fprintf(stderr, "usage: pub <register_pipe_name> <client_pipe_name> <box_name>\n");
		s = read_input(buffer, client_pipe_name, box_name);
	}
	char *register_pipe_name = strdup(buffer);
	if (register_pipe_name == NULL) {
		PANIC("Failed to allocate memory");
	}
	INFO("Input read");

	create_fifo(client_pipe_name);

	client_request_t *request = create_client_request(CODE_REGISTER_PUBLISHER, client_pipe_name, box_name);
	if (send_request_to_server(register_pipe_name, request) != 0) {
		// FIXME should i delete fifo?
		PANIC("Failed to send request to server");
	}

	//INTERNAL what if server rejects - no box example
	free(register_pipe_name);
	client_pipe = start_fifo(client_pipe_name, O_WRONLY);//FIXME check if it worked?
	//threads sending messages?
	while (1) {
		publish_message(client_pipe, get_message());
	}
	return 0;
}

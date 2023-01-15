#include "logging.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <signal.h>

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
    (void)argc;
    (void)argv;
    fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");
    WARN("unimplemented"); // TODO: implement
    return -1;
}

#include "logging.h"
#include "../protocol/default_sizes.h"
#include "../protocol/codes.h"
#include "manager.h"
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include "protocol_manager.h"

static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}

int read_input(char *register_pipe_name, client_pipe_path_t *client_pipe_name, char *mode, char *box_name) {

	char input[MAX_INPUT_SIZE];
	char assert[MAX_INPUT_SIZE];
	if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
		PANIC("Failed to read from stdin");
		return -1;
	}

	ssize_t scan_matches = sscanf(input, "manager %s %s %s %s %s", register_pipe_name, (char *) client_pipe_name, mode,
								  box_name, assert);
	if (scan_matches != 3 && scan_matches != 4) {
		return -1;
	}

	if (!strcmp(mode, "create")) {
		return MODE_CREATE;
	} else if (!strcmp(mode, "remove")) {
		return MODE_REMOVE;
	} else if (!strcmp(mode, "list") && scan_matches == 3) {
		return MODE_LIST;
	}

	return -1;
}

void handle_create_remove_box(char *register_pipe_name, client_pipe_path_t client_pipe_name[MAX_CLIENTPIPE_NAME],
							  char box_name[MAX_BOX_NAME],
							  code_t send_code) {
	// create box
	// make it so that every character in box name is \0
	size_t len = strlen(box_name);
	memset(box_name + len, '\0', MAX_BOX_NAME - len);    //FIXME check for error
	// fills the rest of the box name with '\0'


	create_remove_box_request_t request = build_create_remove_box_request(send_code, *client_pipe_name,
																		  box_name);

// send request
	int fd = open(register_pipe_name, O_WRONLY); // INTERNAL CHECK IF THIS RETURNS -1
	write(fd,
		  &request, sizeof(request));
	INFO("Sent request to server");

// read response on client pipe
	int client_fd = open(*client_pipe_name, O_RDONLY);
	create_remove_box_response_t response;
	read(client_fd,
		 &response, sizeof(create_remove_box_response_t)); // INTERNAL HANDLE READ


	if (response.ret_code == 0) {
		fprintf(stdout,
				"OK\n");
	} else {
		fprintf(stdout,
				"ERROR %s\n", response.error_message); // INTERNAL HANDLE PANIC
	}
}

void handle_list_boxes(char *register_pipe_name, client_pipe_path_t *client_pipe_name) {
	// create box
	list_boxes_request_t request = build_list_boxes_request(*client_pipe_name);

	// send request
	int fd = open(register_pipe_name, O_WRONLY); // INTERNAL CHECK IF THIS RETURNS -1
	write(fd, &request, sizeof(request));
	INFO("Sent request to server");
	// INTERNAL SHOULD I CLOSE THE FD HERE?

	// read response on client pipe
	int client_fd = open(*client_pipe_name, O_RDONLY);
	list_boxes_response_t response;

	do {
		read(client_fd, &response, sizeof(list_boxes_response_t)); // INTERNAL HANDLE READ

		if (response.box_name[0] == '\0') {
			fprintf(stdout, "NO BOXES FOUND\n");
		} else {
			fprintf(stdout, "%s %zu %zu %zu\n", response.box_name, (size_t) response.box_size,
					(size_t) response.n_publishers,
					(size_t) response.n_subscribers);
		}
	} while (response.last == 0);
}


int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    print_usage();
    WARN("unimplemented"); // TODO: implement
    return -1;
}

//criação e remoção da caixa
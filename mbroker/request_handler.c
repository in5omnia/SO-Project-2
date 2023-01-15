
#include "request_handler.h"
#include "../protocol/codes.h"
#include "../protocol/protocol.h"
#include "../utils/logging.h"
#include "../utils/fifo.h"
#include "../fs/operations.h"
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>


extern box_list_t *box_list;	//head of the box list

void handle_request(request_t *request) {
	switch (request->code) {
		case CODE_REGISTER_PUBLISHER:
			handle_publisher(request);
			break;
		case CODE_REGISTER_SUBSCRIBER:
			handle_subscriber(request);
			break;
		case CODE_CREATE_MBOX:
			handle_mbox_creation(request);
			break;
		case CODE_REMOVE_MBOX:
			handle_mbox_removal(request);
			break;
		case CODE_LIST_MBOX:
			handle_mbox_listing(request);
			break;
		default:
			PANIC("No Such Request Code");
	}
}

int write_to_box(int box_fhandle, message_t *message, int client_pipe) {
	if (read(client_pipe, message, MAX_MESSAGE_SIZE) == 0) {
		return -1;    //to be confirmed
	}
	tfs_write(box_fhandle, message->message, MAX_MESSAGE_SIZE);
	return 0;
}


void handle_publisher(request_t *request) {
	box_entry_t *box = box_lookup(request->box_name);
	if (box == NULL) {
		// TODO: REJECT INTERNAL
		INFO("Box does not exist");
		return;
	}
	if (box->n_publishers != 0) {
		INFO("Box already has a publisher");
		//TODO: INTERNAL wait?- check piaza
	}
	box->n_publishers++;

	int box_fhandle = tfs_open(request->box_name, TFS_O_APPEND);
	int client_pipe = start_fifo(request->client_named_pipe_path, O_RDONLY);
	message_t *message = (message_t *) malloc(sizeof(message_t));

	while (box_lookup(request->box_name) != NULL) {    //while box exists
		pthread_mutex_lock(&box->box_entry_condvar_lock);
		if (write_to_box(box_fhandle, message, client_pipe) == -1) {
			box->n_publishers--;
			pthread_mutex_unlock(&box->box_entry_condvar_lock);
			break;
		}
		box->box_size += MAX_MESSAGE_SIZE;
		pthread_cond_broadcast(&box->box_entry_condvar);
		pthread_mutex_unlock(&box->box_entry_condvar_lock);
	}
	// when the box is removed, the subscriber process will end
	free(message);
	close(client_pipe);
	tfs_close(box_fhandle);
	unlink(request->client_named_pipe_path);    //this is will generate SIGPIPE in publisher process
}


void handle_mbox_creation(request_t *request) {
	code_t code = CODE_ANSWER_CREATE_MBOX;
	error_message_t error;
	ret_code_t ret_code;

	// Verify if the box exists
	if (box_lookup(request->box_name)) {
		// Reject
		ret_code = (ret_code_t) -1;
		strcpy(error, BOX_ALREADY_EXISTS_ERROR);
		INFO(BOX_ALREADY_EXISTS_ERROR);
	} else {
		int fd = tfs_open(request->box_name, TFS_O_CREAT);
		if (fd == -1) {
			// Reject
			ret_code = (ret_code_t) -1;
			strcpy(error, BOX_CREATION_ERROR);
			INFO(BOX_CREATION_ERROR);
		} else {
			// Accept
			ret_code = (ret_code_t) 0;
			strcpy(error, BOX_CREATION_SUCCESS);
			box_list_insert(box_entry_create(request->box_name)); // INTERNAL SHOULD WE ASSURE THE NAME?
		}
	}
	// set all remaining bytes of error to \0
	size_t len = strlen(error);
	memset(error + len, '\0', MAX_ERRORMESSAGE_SIZE - len);

	create_remove_box_response_t response = build_create_remove_box_response(code, ret_code, error);

	int client_fifo = open(request->client_named_pipe_path, O_RDWR);
	ssize_t bytes_written = write(client_fifo, &response, sizeof(response));
	if (bytes_written == -1) {
		PANIC("Failed to write to FIFO");
		return;
	}
	close(client_fifo);
}

void handle_mbox_listing(request_t *request) {
	code_t code = CODE_ANSWER_LIST_MBOX;
	last_t last;
	box_name_t box_name;
	box_size_t box_size;
	n_publishers_t n_publishers;
	n_subscribers_t n_subscribers;

	// Iterate over the box list
	box_list_t *box = box_list;
	// TODO WHEN ITS EMPTY

	// Open the client FIFO
	int client_fifo = open(request->client_named_pipe_path, O_WRONLY);
	while (box != NULL) {
		// Build the response
		last = (box->next == NULL) ? 1 : 0;
		memcpy(box_name, box->box_entry->box_name, MAX_BOX_NAME);
		box_size = box->box_entry->box_size;
		n_publishers = box->box_entry->n_publishers;
		n_subscribers = box->box_entry->n_subscribers;

		list_boxes_response_t response = build_list_boxes_response(code, last, box_name, box_size, n_publishers,
																   n_subscribers);

		// Write the response to the client FIFO
		ssize_t bytes_written = write(client_fifo, &response, sizeof(response));
		if (bytes_written == -1) {
			PANIC("Failed to write to FIFO");
			return;
		}

		// Move to the next box
		box = box->next;
	}
	close(client_fifo);

}


void handle_mbox_removal(request_t *request) {
	code_t code = CODE_ANSWER_CREATE_MBOX;
	error_message_t error;
	ret_code_t ret_code;

	// Verify if the box exists
	box_entry_t *box = box_lookup(request->box_name);
	if (box) {
		// Accept & delete
		if (tfs_unlink(request->box_name) == -1) {
			ret_code = (ret_code_t) -1;
			strcpy(error, BOX_REMOVAL_ERROR);
			INFO(BOX_REMOVAL_ERROR);
		} else {
			ret_code = (ret_code_t) 0;
			strcpy(error, BOX_REMOVAL_SUCCESS);
			INFO("Box removed");
		}
	} else {
		ret_code = (ret_code_t) -1;
		strcpy(error, BOX_NOT_FOUND_ERROR);
		INFO(BOX_NOT_FOUND_ERROR);
	}

	size_t len = strlen(error);
	memset(error + len, '\0', MAX_ERRORMESSAGE_SIZE - len);

	create_remove_box_response_t response = build_create_remove_box_response(code, ret_code, error);

	int client_fifo = open(request->client_named_pipe_path, O_RDWR); // INTERNAL RDWR?
	ssize_t bytes_written = write(client_fifo, &response, sizeof(response));
	if (bytes_written == -1) {
		PANIC("Failed to write to FIFO");
		return;
	}
	close(client_fifo);
}


char *get_each_message(const char *buffer, int size) {
	// get the length of the destination string
	char *message = (char *) malloc(sizeof(char) * (size_t) (size));
	if (message == NULL) {
		printf("malloc failed");
		return NULL;
	}
	char *temp = message;

	for (int i = 0; i < size; i++) {
		*temp = *(buffer + i);
		temp++;
	}

	// return the message
	return message;
}


int read_from_box(int box_fhandle, char *buffer, int client_pipe) {

	tfs_read(box_fhandle, buffer, sizeof(client_pipe_path_t));
	int block_size = (int) strlen(buffer);
	char *temp = buffer;
	for (int i = 0; i < block_size; i += MAX_MESSAGE_SIZE) {
		message_t *message = (message_t *) malloc(sizeof(message_t));
		message->code = CODE_SUBSCRIBER_RECEIVE_MSG;
		strcpy(message->message, get_each_message(temp, MAX_MESSAGE_SIZE));
		// write message to pipe of subscriber
		write(client_pipe, message, MAX_MESSAGE_SIZE);
		if (errno == EPIPE) {
			INFO("Client disconnected");
			return -1;
		}
		temp = temp + MAX_MESSAGE_SIZE;
	}
	return 0;
}


void handle_subscriber(request_t *request) {
	box_entry_t *box = box_lookup(request->box_name);
	if (box == NULL) {
		// TODO: REJECT
		INFO("Box does not exist");
		return;
	}
	box->n_subscribers++;

	int box_fhandle = tfs_open(request->box_name, TFS_O_CREAT);
	char buffer[DEFAULT_BLOCK_SIZE];
	int client_pipe = start_fifo(request->client_named_pipe_path, O_WRONLY);

	pthread_mutex_lock(&box->box_entry_condvar_lock);
	while (box_lookup(request->box_name) != NULL) {
		//its always reading bc publishers can publish more	FIXME
		if (read_from_box(box_fhandle, buffer, client_pipe) == -1) {
			INFO("Client disconnected");
			break;
		};
		pthread_cond_wait(&box->box_entry_condvar, &box->box_entry_condvar_lock);
	}
	// when the box is removed, the subscriber process will end
	box->n_subscribers--;
	pthread_mutex_unlock(&box->box_entry_condvar_lock);
	close(client_pipe);    // this will generate eof in subscriber process
	tfs_close(box_fhandle);
	//unlink(request->client_named_pipe_path);    //this is will generate SIGPIPE in subscriber process
}

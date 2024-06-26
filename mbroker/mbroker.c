#include "../producer-consumer/producer-consumer.h"
#include "../protocol/default_sizes.h"
#include "logging.h"
#include "operations.h"
#include "protocol_mbroker.h"
#include "stdlib.h"
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../utils/fifo.h"
#include <string.h>
#include "request_handler.h"
#include "box_list.h"
#include <signal.h>

char *register_pipe_name;
int register_fifo;
pc_queue_t *pc_queue;

pthread_mutex_t box_list_mutex = PTHREAD_MUTEX_INITIALIZER;
box_list_t *box_list = NULL;
int max_sessions;
pthread_t *threads;

void exit_process(int sig) {
	INFO("Exiting process, %d", sig);
	if (box_list) {
		box_list_destroy(box_list);
	}
	if (pthread_mutex_destroy(&box_list_mutex)!=0) {
		PANIC("Failed to destroy box list mutex");
	}
	pcq_destroy(pc_queue);
	free(pc_queue);
	if (close(register_fifo)!=0)
		PANIC("Failed to close register fifo");
	if (unlink(register_pipe_name)!=0)
		PANIC("Failed to unlink register pipe");
	for (int i=0; i<max_sessions; i++){
		pthread_kill(threads[i], SIGKILL);
	}
	exit(0);
}

int init_fs() {
	INFO("Starting up");
	tfs_params params = tfs_default_params();
	if (tfs_init(&params) != 0) {
		PANIC("Failed to initialize filesystem");
	}
	INFO("Filesystem initialized");

	return 0;
}

int destroy_fs() {
	if (tfs_destroy() != 0) {
		PANIC("Failed to destroy filesystem");
	}

	INFO("Filesystem destroyed");

	return 0;
}


int read_input(char *buffer) {
	char input[MAX_INPUT_SIZE];
	char assert[MAX_INPUT_SIZE];
	if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
		PANIC("Failed to read from stdin");
		return -1;
	}
	ssize_t scan_matches = sscanf(input, "mbroker %s %d %s", buffer, &max_sessions, assert);
	if (scan_matches != 2) {
		return -1;
	}
	return 0;
}


void *worker_thread(void *session_id) {
	(void) session_id;
	while (1) {

		request_t *request = (request_t *) pcq_dequeue(pc_queue);

		handle_request(request);

		free(request);
	}

	return NULL;
}

pthread_t start_worker_thread(int session_id) {
	pthread_t thread;
	if (pthread_create(&thread, NULL, worker_thread, &session_id) != 0) {
		PANIC("Failed to create thread");
	}
	return thread;
}


int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, exit_process);

	char buffer[MAX_INPUT_SIZE];

	INFO("Reading input");
	int s = read_input(buffer);
	while (s == -1) {
		fprintf(stderr, "usage: mbroker <register_pipe_name> <max_sessions>\n");
		s = read_input(buffer);
	}
	INFO("Input read");

	register_pipe_name = strdup(buffer);
	if (register_pipe_name == NULL) {
		PANIC("Failed to allocate memory");
	}

	if (init_fs() != 0) { // This might not occur but it's good practice to check
		PANIC("Failed to initialize filesystem");
	}


	// PCQ
	pc_queue = (pc_queue_t *) malloc(sizeof(pc_queue_t));
	if (pc_queue == NULL) {
		PANIC("Failed to allocate memory");
		return -1;
	}
	if (pcq_create(pc_queue, SESSION_CAPACITY * (size_t) max_sessions) == -1) {
		PANIC("Failed to allocate pc_queue");
		return -1;
	}
	INFO("Created pc_queue");


	// Start all worker threads
	threads = malloc((size_t) max_sessions * sizeof(pthread_t));
	for (int i = 0; i < max_sessions; i++) {
		threads[i] = start_worker_thread(i);
	}

	// Make Register FIFO
	create_fifo(register_pipe_name);
	register_fifo = start_fifo(register_pipe_name, O_RDWR);
	INFO("Register fifo created: %s", register_pipe_name);

	while (1) {
		request_t *request = (request_t *) malloc(
				sizeof(request_t));
		if (request == NULL) {
			PANIC("Failed to allocate memory");
		}
		parse_requests(register_fifo, request);
		pcq_enqueue(pc_queue, request);

	}

	return -1;
}

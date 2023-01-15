#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "logging.h"
#include "operations.h"
#include "default_sizes.h"


int init_fs() {
	INFO("Starting up");
	tfs_params params = tfs_default_params(); // TODO: maybe adjust accordingly
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


int making_fifo(char *pipe_name) {
	int fd;
	if (mkfifo(pipe_name, 0666) == -1) {    //FIXME 0666
		PANIC("Failed to create fifo");
	}
	fd = open(pipe_name, O_RDWR);    //FIXME bc RDWR - n sei se tem de ser dif instancias
	if (fd == -1)
		PANIC("Failed to open fifo"); // TODO: add error handling/Reason

	INFO("FIFO opened in RDWR:", pipe_name);

	return fd;
}


void worker_thread(void* session_id) {
    INFO("Worker thread %d started", (int) session_id);
	while (1) {

		request_t *request = (request_t *) pcq_dequeue(pc_queue);

		handle_request(request, box_list);

		free(request);
	}


}

pthread_t start_worker_thread(int session_id) {
	pthread_t thread;
	if (pthread_create(&thread, NULL, worker_thread, session_id) != 0) {
		PANIC("Failed to create thread");
	}
	return thread;
}

// TODO: Mbroker stops with SIGINT
int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	// TODO read input from stdin and parse it
	if (0) { // TODO check if it's a valid input
		fprintf(stderr, "usage: mbroker <pipename>\n");
	}
	WARN("unimplemented"); // TODO: implement


	if (init_fs() != 0) { // INTERNAL This might not occur but it's a good practice to check
		PANIC("Failed to initialize filesystem");
	}

// Make Register FIFO
	int register_fifo = making_fifo("pi2pe"); // FIXME REPLACE WITH ACTUAL FIFO


	while (read(register_fifo, &msg, sizeof(msg)) > 0) {

	}


	close(register_fifo);
	unlink("pi2pe"); // FIXME REPLACE WITH ACTUAL FIFO

	return -1;
}

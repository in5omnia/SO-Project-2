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


int destroy_fs(){
	if (tfs_destroy() != 0) {
		PANIC("Failed to destroy filesystem");
	}

	INFO("Filesystem destroyed");

	return 0;
}


int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // TODO read input from stdin and parse it

	fprintf(stderr, "usage: mbroker <pipename>\n");
	WARN("unimplemented"); // TODO: implement


	if (init_fs() != 0) { // INTERNAL This might not occur but it's a good practice to check
		PANIC("Failed to initialize filesystem");
	}

	// TODO open register fifo


    return -1;
}

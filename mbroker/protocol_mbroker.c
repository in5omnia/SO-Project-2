#include "protocol_mbroker.h"
#include "../protocol/codes.h"
#include "../protocol/default_sizes.h"
#include "logging.h"
#include <unistd.h>

void parse_requests(int fifo, request_t *request) {
  ssize_t bytes = read(fifo, request, sizeof(request_t));
  if (bytes == -1) {
	  PANIC("Can't parse request");
  }
  INFO("Parsed request with OP_CODE: %d", request->code);
}
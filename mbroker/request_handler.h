
#ifndef PROJETO_SO_2_REQUEST_HANDLER_H
#define PROJETO_SO_2_REQUEST_HANDLER_H

#include "protocol_mbroker.h"
#include "box_list.h"
#include "../protocol/protocol.h" // INTERNAL FIXME SHOULDN'T BE HERE

#define BOX_ALREADY_EXISTS_ERROR "Box already exists"
#define BOX_CREATION_ERROR "Box creation error"
#define BOX_CREATION_SUCCESS "\0"
#define BOX_REMOVAL_ERROR "Box removal error"
#define BOX_REMOVAL_SUCCESS "\0"
#define BOX_NOT_FOUND_ERROR "Box not found"

void handle_request(request_t* request);

void handle_publisher(request_t* request);

void handle_subscriber(request_t* request);

void handle_mbox_creation(request_t* request);

void handle_mbox_removal(request_t* request);

void handle_mbox_listing(request_t* request);


#endif //PROJETO_SO_2_REQUEST_HANDLER_H

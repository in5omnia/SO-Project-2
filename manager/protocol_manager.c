//
// Created by Beatriz Gavilan on 14/01/2023.
//

#include <string.h>
#include "protocol_manager.h"
#include "../protocol/codes.h"
#include "../protocol/default_sizes.h"


create_remove_box_request_t
build_create_remove_box_request(code_t code, client_pipe_path_t client_named_pipe_path, box_name_t box_name) {
	create_remove_box_request_t request;
	request.code = code;
	memcpy(request.client_named_pipe_path, client_named_pipe_path, MAX_CLIENTPIPE_NAME);
	memcpy(request.box_name, box_name, MAX_BOX_NAME);
	return request;
}

create_remove_box_response_t
build_create_remove_box_response(code_t code, ret_code_t ret_code, error_message_t error_message) {
	create_remove_box_response_t response;
	response.code = code;
	response.ret_code = ret_code;
	memcpy(response.error_message, error_message, MAX_ERRORMESSAGE_SIZE);
	return response;
}


list_boxes_request_t build_list_boxes_request(client_pipe_path_t client_named_pipe_path) {
	list_boxes_request_t request;
	request.code = CODE_LIST_MBOX;
	memcpy(request.client_named_pipe_path, client_named_pipe_path, MAX_CLIENTPIPE_NAME);
	return request;
}

list_boxes_response_t
build_list_boxes_response(code_t code, last_t last, box_name_t box_name, box_size_t box_size, n_publishers_t n_pub,
						  n_subscribers_t n_sub) {

	struct list_boxes_responses_t response;
	response.code = code;
	response.last = last;
	memcpy(response.box_name, box_name, MAX_BOX_NAME);
	response.box_size = box_size;
	response.n_publishers = n_pub;
	response.n_subscribers = n_sub;

	return response;

}




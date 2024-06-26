#include <stdlib.h>
#include <string.h>
#include "../utils/logging.h"
#include "box_list.h"


extern box_list_t *box_list;	//head of the box list

// Creates a new box entry
box_entry_t *box_entry_create(char box_name[MAX_BOX_NAME]) {
	box_entry_t *new_box = (box_entry_t *) malloc(sizeof(box_entry_t));
	if (new_box == NULL) {
		PANIC("Could not allocate memory for box entry");
		return NULL;
	}
	new_box->box_size = 0;
	new_box->n_subscribers = 0;
	new_box->n_publishers = 0;
	memcpy(new_box->box_name, box_name, MAX_BOX_NAME);

	// Init condvar and mutex
	pthread_mutex_init(&new_box->box_entry_condvar_lock, NULL);
	pthread_cond_init(&new_box->box_entry_condvar, NULL);

	return new_box;
}

// Creates a new box list
void box_entry_destroy(box_entry_t *box_entry) {
	// Destroy condvar and mutex
	if (pthread_mutex_destroy(&box_entry->box_entry_condvar_lock) !=0 )
		PANIC("Could not destroy box entry condvar lock");
	if (pthread_cond_destroy(&box_entry->box_entry_condvar)!=0)
		PANIC("Could not destroy box entry condvar");
	free(box_entry);
}

// Inserts a box entry into the box list
void box_list_insert(box_entry_t *box_entry) {
	// Insert to linked list
	box_list_t *new_head = (box_list_t *) malloc(sizeof(box_list_t));
	if (new_head == NULL) {
		PANIC("Could not allocate memory for box list");
		return;
	}
	new_head->box_entry = box_entry;
	if (box_list) {
		new_head->next = box_list;
	} else {
		new_head->next = NULL;
	}
	box_list = new_head;
}

// Removes a box entry from the box list
void box_list_remove(char box_name[MAX_BOX_NAME]) {
	// Look for box entry
	box_list_t *prev = NULL;
	box_list_t *curr = box_list;
	while (curr != NULL) {
		if (!strcmp(curr->box_entry->box_name, box_name)) {
			// Found box entry
			if (prev == NULL) {
				// First element of the list
				box_list = curr->next;
			} else {
				prev->next = curr->next;
			}
			box_entry_destroy(curr->box_entry);
			free(curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}


box_entry_t *box_lookup(char box_name[MAX_BOX_NAME]) {
	// check if path starts with '/'
	char *new_pathname = (char*)malloc(MAX_BOX_NAME+1);
	if (box_name[0] != '/') {
		new_pathname[0] = '/';
		memcpy(new_pathname + 1, box_name, MAX_BOX_NAME);
	} else {
		memcpy(new_pathname, box_name, MAX_BOX_NAME);
	}

	box_list_t *curr = box_list;
	while (curr != NULL) {
		if (!strcmp(curr->box_entry->box_name, new_pathname)) {
			// Found box entry
			free(new_pathname);
			return curr->box_entry;

		}
		curr = curr->next;
	}
	free(new_pathname);
	return NULL;
}

void box_list_destroy() {
    box_list_t *curr = box_list;
    while (curr != NULL) {
        box_list_t *next = curr->next;
        box_entry_destroy(curr->box_entry);
        free(curr);
        curr = next;
    }
}



#ifndef PROJETO_SO_2_BOX_LIST_H
#define PROJETO_SO_2_BOX_LIST_H

#include "../protocol/default_sizes.h"
#include <pthread.h>


typedef struct box_entry_t {
	char box_name[MAX_BOX_NAME];
	int n_subscribers;
	int n_publishers;
	int box_size;
	pthread_mutex_t box_entry_condvar_lock;
	pthread_cond_t box_entry_condvar;
} box_entry_t;


// Linked list of box entry
typedef struct box_list_t {
	box_entry_t *box_entry;
	struct box_list_t *next;
} box_list_t;


// Creates a new box entry
box_entry_t *box_entry_create(char box_name[MAX_BOX_NAME]);

// Destroy box entry
void box_entry_destroy(box_entry_t *box_entry);

// Creates a new box list
box_list_t *box_list_create();

// Inserts a box entry into the box list
void box_list_insert(box_entry_t *box_entry);

// Removes a box entry from the box list
void box_list_remove(char box_name[MAX_BOX_NAME]);

/* Checks if a box exists
 * Returns: box_entry to that box if it exists, NULL otherwise
 */
box_entry_t *box_lookup(char box_name[MAX_BOX_NAME]);

// Destroys a box list
void box_list_destroy();

#endif //PROJETO_SO_2_BOX_LIST_H

#include "producer-consumer.h"
#include <stdlib.h>
#include "logging.h"


int pcq_create(pc_queue_t *queue, size_t capacity){
	queue->pcq_buffer = malloc(capacity * sizeof(void*));
	if (queue->pcq_buffer == NULL) {
		PANIC("Failed to allocate memory for pcq_buffer");
		return -1;
	}
	queue->pcq_capacity = capacity;
	queue->pcq_current_size = 0;
	queue->pcq_head = 0;
	queue->pcq_tail = 0;

	pthread_mutex_init(&queue->pcq_current_size_lock, NULL);
	pthread_mutex_init(&queue->pcq_head_lock, NULL);
	pthread_mutex_init(&queue->pcq_tail_lock, NULL);
	pthread_mutex_init(&queue->pcq_pusher_condvar_lock, NULL);
	pthread_mutex_init(&queue->pcq_popper_condvar_lock, NULL);

	pthread_cond_init(&queue->pcq_pusher_condvar, NULL);
	pthread_cond_init(&queue->pcq_popper_condvar, NULL);

	return 0;
}



int pcq_enqueue(pc_queue_t *queue, void *elem){
	pthread_mutex_lock(&queue->pcq_pusher_condvar_lock);
	while(queue->pcq_current_size == queue->pcq_capacity){
		//waiting for available space
		pthread_cond_wait(&queue->pcq_pusher_condvar, &queue->pcq_pusher_condvar_lock);
	}
	pthread_mutex_unlock(&queue->pcq_pusher_condvar_lock);

	pthread_mutex_lock(&queue->pcq_current_size_lock);
	queue->pcq_current_size++;
	pthread_mutex_unlock(&queue->pcq_current_size_lock);
	//update the head
	pthread_mutex_lock(&queue->pcq_head_lock);
	queue->pcq_buffer[queue->pcq_head] = elem;
	queue->pcq_head = (queue->pcq_head + 1) % queue->pcq_capacity;
	pthread_mutex_unlock(&queue->pcq_head_lock);
	//warn popper that there's new requests
	pthread_mutex_lock(&queue->pcq_popper_condvar_lock);
	pthread_cond_signal(&queue->pcq_popper_condvar);
	pthread_mutex_unlock(&queue->pcq_popper_condvar_lock);

	return 0;
}



void *pcq_dequeue(pc_queue_t *queue){
	pthread_mutex_lock(&queue->pcq_popper_condvar_lock);
	while(queue->pcq_current_size == 0){
		//waiting for request
		pthread_cond_wait(&queue->pcq_popper_condvar, &queue->pcq_popper_condvar_lock);
	}
	pthread_mutex_unlock(&queue->pcq_popper_condvar_lock);

	pthread_mutex_lock(&queue->pcq_current_size_lock);
	queue->pcq_current_size--;
	pthread_mutex_unlock(&queue->pcq_current_size_lock);
	//update the tail
	pthread_mutex_lock(&queue->pcq_tail_lock);
	void *elem = queue->pcq_buffer[queue->pcq_tail];
	queue->pcq_tail = (queue->pcq_tail + 1) % queue->pcq_capacity;
	pthread_mutex_unlock(&queue->pcq_tail_lock);
	//warn pusher that there's available space
	pthread_mutex_lock(&queue->pcq_pusher_condvar_lock);
	pthread_cond_signal(&queue->pcq_pusher_condvar);
	pthread_mutex_unlock(&queue->pcq_pusher_condvar_lock);

	return elem;
}



int pcq_destroy(pc_queue_t *queue){
	pthread_mutex_destroy(&queue->pcq_current_size_lock);
	pthread_mutex_destroy(&queue->pcq_head_lock);
	pthread_mutex_destroy(&queue->pcq_tail_lock);
	pthread_mutex_destroy(&queue->pcq_pusher_condvar_lock);
	pthread_mutex_destroy(&queue->pcq_popper_condvar_lock);
	pthread_cond_destroy(&queue->pcq_pusher_condvar);
	pthread_cond_destroy(&queue->pcq_popper_condvar);
	size_t elem = queue->pcq_tail;
	size_t size = queue->pcq_current_size;
	for (int i=0; i < size; i++){
		free(queue->pcq_buffer[elem]);
		elem = (elem + 1) % queue->pcq_capacity;
	}
	free(queue->pcq_buffer);
	return 0;
}

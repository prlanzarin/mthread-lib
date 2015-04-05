#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"

int queue_size(TCB_t *queue)
{
	int size;
	TCB_t *ptr;

	size = 0;
	ptr = queue;
	while (ptr != NULL) {
		ptr = ptr->next;
		size++;
	}
	return size;
}

/* Adiciona elem ao início da fila */
TCB_t *enqueue(TCB_t *tcb, TCB_t *queue)
{
	tcb->next = queue;
	return tcb;
}

/* Remove o último elemento da fila (primeiro a ser adicionado) e retorna-o */
TCB_t *dequeue(TCB_t *queue)
{
	TCB_t *ptr, *prev;
	ptr = queue;
	prev = ptr;
	if (ptr == NULL)
		return ptr;

	while (ptr->next != NULL) {
		ptr = ptr->next;
		prev = ptr;
	}

	prev->next = NULL;
	return ptr;
}	


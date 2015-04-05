#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"

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
	while (ptr->next != NULL) {
		ptr = ptr->next;
		prev = ptr;
	}

	prev->next = NULL;
	return ptr;
}	


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

/* busca por 'tid' na fila. Retorna um ponteiro pro TCB_t encontrado, ou
 * NULL caso não o encontre.
 */
TCB_t *search_queue(int tid, TCB_t *queue)
{
	TCB_t *ptr;
	ptr = queue;
	if (ptr == NULL)
		return NULL;
	while (ptr != NULL) {
		if (ptr->tid == tid) {
			return ptr;
		}
		ptr = ptr->next;
	}
	return NULL; /* não encontrou */
}

/* Adiciona elem ao início da fila */
void enqueue(TCB_t *tcb, TCB_t **queue)
{
	tcb->next = *queue;
	*queue = tcb;
}

/* Remove o último elemento da fila (primeiro a ser adicionado) e retorna-o */
TCB_t *dequeue(TCB_t **queue)
{
	TCB_t *ptr, *prev;
	ptr = *queue;
	prev = ptr;
	if (ptr == NULL)
		return ptr;
	if (ptr->next == NULL) {
		*queue = NULL;
		return ptr;
	}

	while (ptr->next != NULL) {
		prev = ptr;
		ptr = ptr->next;
	}

	prev->next = NULL;
	return ptr;
}

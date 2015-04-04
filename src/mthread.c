#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/mdata.h"
#include "../include/mthread.h"
#define NUM_PRIO_LVLS 3

TCB_t *apto[NUM_PRIO_LVLS] = {NULL, NULL, NULL};
TCB_t *executando = NULL;
TCB_t *bloqueado = NULL;

int initialize()
{
	ucontext_t cur_context;

	getcontext(&cur_context);
	TCB_t *main_tcb;	   
	main_tcb = malloc(sizeof(TCB_t));
	main_tcb->tid = 0;
	main_tcb->prio = 0; /* prioridade alta */
	main_tcb->context = cur_context;
	main_tcb->prev = NULL;
	main_tcb->next = NULL;
	executando = main_tcb;
	return 0;
}

int mcreate(int prio, void (*start)(void*), void *arg)
{
	ucontext_t cur_context, new_context;

	getcontext(&cur_context);
	if (executando == NULL) {
	/* primeira função de mthread chamada. */
		initialize();
	}

	char new_stack[SIGSTKSZ];
}

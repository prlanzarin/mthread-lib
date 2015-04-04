#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/mdata.h"
#include "../include/mthread.h"
#define NUM_PRIO_LVLS 3

TCB_t *apto[NUM_PRIO_LVLS] = {NULL, NULL, NULL};
TCB_t *executando = NULL;
TCB_t *bloqueado = NULL;

int mcreate(int prio, void (*start)(void*), void *arg)
{
	ucontext_t cur_context, new_context;

	getcontext(&cur_context);
	if (executando == NULL) {
	/* mcreate é chamado pela primeira vez. Cria um TCB_t para main_tcb e
	 * coloca na fila de apto (ou executando? Logo saberemos) */
		TCB_t *main_tcb;	   
		main_tcb = malloc(sizeof(TCB_t));
		main_tcb->tid = 0;
		main_tcb->prio = 0;
		main_tcb->context = cur_context;
		main_tcb->prev = NULL;
		main_tcb->next = NULL;
		apto[0] = main_tcb;
	}

	char new_stack[SIGSTKSZ];
}

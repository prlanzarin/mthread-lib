#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/mdata.h"
#include "../include/mthread.h"
#define NUM_PRIO_LVLS 3

TCB_t *apto[NUM_PRIO_LVLS] = {NULL, NULL, NULL};
TCB_t *executando = NULL;
TCB_t *bloqueado = NULL;
TCB_t *main_tcb = NULL;
int tids = 0;
int scheduler();
int initialize();

/* Aloca espaço pra TCB da main thread. Associa a função 'scheduler' ao
 * contexto da 'main_tcb' e a coloca no estado 'executando' */
int initialize()
{
	ucontext_t *context;
	char stack[SIGSTKSZ];

	main_tcb = malloc(sizeof(TCB_t));
	main_tcb->tid = tids++;
	main_tcb->prio = 0; /* prioridade alta */
	main_tcb->prev = NULL;
	main_tcb->next = NULL;

	getcontext(&(main_tcb->context));
	context = &(main_tcb->context);
	context->uc_link = NULL; /* FIXME */
	context->uc_stack.ss_sp = stack;
	context->uc_stack.ss_size = sizeof(stack);

	makecontext(context, (void (*)(void)) scheduler, 0);

	executando = main_tcb;
	return 0;
}

/* a principal função da biblioteca, responsável por gerenciar as tarefas */
int scheduler()
{
	printf("I'm the scheduler\n");
	return 0;
}

int mcreate(int prio, void (*start)(void*), void *arg)
{
	ucontext_t *context;
	TCB_t *tcb;
	char stack[SIGSTKSZ];

	if (prio < 0 || prio > NUM_PRIO_LVLS - 1) {
		printf("error: invalid priority level: %d\n", prio);
		return -1;
	}

	if (main_tcb == NULL) {
	/* primeira função de mthread chamada. */
		initialize();
	}

	/* Cria nova TCB e insere na fila de prioridades */
	tcb = malloc(sizeof(TCB_t));
	tcb->tid = tids++;
	tcb->prio = prio;
	tcb->prev = NULL;
	tcb->next = apto[prio];
	apto[prio] = tcb;

	/* cria novo contexto a partir a partir do atual. */
	/* context é só pra facilitar o acesso a tcb->context */
	getcontext(&(tcb->context));
	context = &(tcb->context);
	context->uc_link = &(main_tcb->context);
	context->uc_stack.ss_sp = stack;
	context->uc_stack.ss_size = sizeof(stack);

	return tcb->tid;
}

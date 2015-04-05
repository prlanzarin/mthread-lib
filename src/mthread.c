#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/mdata.h"
#include "../include/mthread.h"
#define NUM_PRIO_LVLS 3
#define MAX_THREADS 64
enum state_t { CRIACAO, APTO, EXECUCAO, BLOQUEADO, TERMINO };

TCB_t *apto[NUM_PRIO_LVLS] = {NULL, NULL, NULL};
TCB_t *executando = NULL;
TCB_t *bloqueado = NULL;
TCB_t main_tcb;
TCB_t *concluido = NULL;
int tids = 0;
int scheduler();
int initialize();

/* Aloca espaço pra TCB da main thread. Associa a função 'scheduler' ao
 * contexto da 'main_tcb' e a coloca no estado 'executando' */
int initialize()
{
	char stack[SIGSTKSZ];

	main_tcb.tid = tids++;
	main_tcb.prio = 0; /* prioridade alta */
	main_tcb.state = CRIACAO; /* FIXME devemos colocar main em executanto */
	main_tcb.prev = NULL;
	main_tcb.next = NULL;

	getcontext(&main_tcb.context);
	main_tcb.context.uc_link = NULL; /* FIXME */
	main_tcb.context.uc_stack.ss_sp = stack;
	main_tcb.context.uc_stack.ss_size = sizeof(stack);

	makecontext(&main_tcb.context, (void (*)(void)) scheduler, 0);

//	executando = &main_tcb;
	return 0;
}

int dispatch(TCB_t *task)
{
	executando = task;
	setcontext(&task->context);
	return -1; /* algo deu errado */
}

/* a principal função da biblioteca, responsável por gerenciar as tarefas */
int scheduler()
{
	int i;
	TCB_t *task;
	getcontext(&main_tcb.context);
	printf("I'm the scheduler\n");
	if (executando != NULL) {
	/* uma tarefa só chega aqui diretamente após término */
		executando->state = TERMINO;
		enqueue(executando, concluido);
	}
	i = 0;
	while (i < NUM_PRIO_LVLS && (task = dequeue(apto[i])) != NULL) {
		dispatch(task);
		i++;
	}
	printf("All queues empty. Leaving. \n");
	exit(0);
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

	if (executando == NULL) {
	/* primeira função de mthread chamada. */
		initialize();
	}

	/* Cria nova TCB e insere na fila de prioridades */
	tcb = malloc(sizeof(TCB_t));
	tcb->tid = tids++;
	tcb->prio = prio;
	tcb->prev = NULL;
	apto[prio] = enqueue(tcb, apto[prio]);

	/* cria novo contexto a partir a partir do atual. */
	/* context é só pra facilitar o acesso a tcb->context */
	getcontext(&tcb->context);
	context = &tcb->context;
	context->uc_link = &(main_tcb.context);
	context->uc_stack.ss_sp = stack;
	context->uc_stack.ss_size = sizeof(stack);
	makecontext(context, (void (*)(void)) start, 1, arg);

	return tcb->tid;
}

int scan_tid(int tid, TCB_t *ptr)
{
	if (ptr == NULL)
		return -1;
	if (ptr->tid == tid)
		return tid;
	else
		return scan_tid(tid, ptr->next);

}

int mwait(int tid)
{
	if (scan_tid(tid, concluido) > -1)
		return 0; /* concluiu */
	setcontext(&main_tcb.context);
	return -1; /* nunca deve ser invocado */
}

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
TCB_t *termino = NULL;
int tids = 1;
ucontext_t sched_context;
char sched_stack[SIGSTKSZ];
int scheduler();
int initialize();
int first_call = 0;

void update_main_context()
{
	getcontext(&main_tcb.context);
}

/* Aloca espaço pra TCB da main thread. Inicializa o contexto do
 * escalonador e associa a função 'scheduler' à sua ativação.
 */
int initialize()
{
	char stack[SIGSTKSZ];
	first_call = 1;
	/* creates scheduler context */
	getcontext(&sched_context);
	sched_context.uc_link = NULL; /* FIXME */
	sched_context.uc_stack.ss_sp = sched_stack;
	sched_context.uc_stack.ss_size = sizeof(sched_stack);
	makecontext(&sched_context, (void (*)(void)) scheduler, 0);

	main_tcb.tid = 0;
	main_tcb.prio = 0; /* prioridade alta */
	main_tcb.state = APTO;
	main_tcb.prev = NULL;

	/*
	getcontext(&main_tcb.context);
	main_tcb.context.uc_link = &sched_context; 
	main_tcb.context.uc_stack.ss_sp = stack;
	main_tcb.context.uc_stack.ss_size = sizeof(stack);

	makecontext(&main_tcb.context, (void (*)(void)) scheduler, 0);
	*/
//	enqueue(&main_tcb, apto[0]);

	executando = &main_tcb;
	return 0;
}


int dispatch(TCB_t *task)
{
	printf("executing task %d\n", task->tid);
	task->state = EXECUCAO;
	executando = task;
	setcontext(&task->context);
	return -1; /* algo deu errado */
}

/* a principal função da biblioteca, responsável por gerenciar as tarefas */
int scheduler()
{
	int i;
	TCB_t *task;

	if (executando != NULL) {
	/* uma tarefa só chega aqui diretamente após término */
		if (executando->tid == 0) {
			printf("running main thread\n");
			enqueue(&main_tcb, apto[0]); /* não mata a main thread */
		} else {
			printf("finishing task %d\n", executando->tid);
			executando->state = TERMINO;
			free(executando->context.uc_stack.ss_sp);
			enqueue(executando, termino);
		}
	}
	i = 0;
	/* checa fila de aptos em ordem (alta -> baixa) */
	while (i < NUM_PRIO_LVLS) {
		if ((task = dequeue(&apto[i])) == NULL)
			i++;
		else
			dispatch(task);
	}
	
	if ((task = dequeue(&bloqueado)) == NULL) {
		printf("All queues empty. Leaving. \n");
		return 0;
	}
	dispatch(task);
	return 0;
}

int mcreate(int prio, void *(*start)(void*), void *arg)
{
	ucontext_t *context;
	TCB_t *tcb;
	char *stack;
	stack = malloc(sizeof(char) * SIGSTKSZ);

	if (prio < 0 || prio > NUM_PRIO_LVLS - 1) {
		printf("error: invalid priority level: %d\n", prio);
		return -1;
	}


	/* Cria nova TCB e insere na fila de prioridades */
	tcb = malloc(sizeof(TCB_t));
	tcb->tid = tids++;
	tcb->prio = prio;
	tcb->state = APTO;
	tcb->prev = NULL;
	apto[prio] = enqueue(tcb, apto[prio]);

	/* cria novo contexto a partir do atual. */
	/* 'context' é só pra facilitar o acesso a tcb->context */
	getcontext(&tcb->context);
	context = &tcb->context;
	context->uc_link = &sched_context;
	context->uc_stack.ss_sp = stack;
	context->uc_stack.ss_size = sizeof(char) * SIGSTKSZ;
	makecontext(context, (void (*)(void)) start, 1, arg);

	if (first_call == 0) {
	/* primeira função de mthread chamada. */
	/* inicializa ao fim para que a main thread esteja associada
	 * ao fluxo principal do programa
	 */
		initialize();
	}
//	swapcontext(&main_tcb.context, &sched_context);
	update_main_context();
	return tcb->tid;
}



int mwait(int tid)
{
	int i, found = 0;
	TCB_t *ptr;
	/* bloqueia main thread */
	main_tcb.state = BLOQUEADO;
	enqueue(&main_tcb, bloqueado);
	executando = NULL;

	/* busca primeiro nas filas de apto */
	while (i < NUM_PRIO_LVLS && found == 0) {
		if ((ptr = search_queue(tid, apto[0])) != NULL) {
			found = 1;
		}
		i++;
	}
	if (found == 0)
		ptr = search_queue(tid, bloqueado);
	if (found == 0)
		ptr = search_queue(tid, termino);
	if (ptr == NULL) {
		printf("tid não encontrado\n");
		return -1;
	}
 
	ptr->context.uc_link = &main_tcb.context;

	swapcontext(&main_tcb.context, &sched_context);
	/* tid terminou */
	main_tcb.state = APTO;
	enqueue(&main_tcb, apto[0]);
	swapcontext(&main_tcb.context, &sched_context);
	return 0;
}

int myield(void)
{
	executando->state = APTO;
	enqueue(executando, apto[executando->tid]);
	if (setcontext(&main_tcb.context) == -1)
		return -1;
	update_main_context();
	return 0;
}

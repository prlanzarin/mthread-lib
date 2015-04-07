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
/* contexto do escalonador */
ucontext_t sched_context;
char sched_stack[SIGSTKSZ];
int scheduler();

ucontext_t terminate_context;
char terminate_stack[SIGSTKSZ];

int initialize();
int first_call = 0;
void terminate();


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

	getcontext(&terminate_context);
	terminate_context.uc_link = NULL; /* FIXME */
	terminate_context.uc_stack.ss_sp = terminate_stack;
	terminate_context.uc_stack.ss_size = sizeof(terminate_stack);
	makecontext(&terminate_context, (void (*)(void)) terminate , 0);


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

void terminate()
{
	printf("terminating %d\n", executando->tid);
	executando->state = TERMINO;
	free(executando->context.uc_stack.ss_sp);
	enqueue(executando, &termino);
	setcontext(&sched_context);
}

/* a principal função da biblioteca, responsável por gerenciar as tarefas */
int scheduler()
{
	int i;
	TCB_t *task;

	i = 0;
	/* checa fila de aptos em ordem (alta -> baixa) */
	while (i < NUM_PRIO_LVLS) {
		if ((task = dequeue(&apto[i])) == NULL)
			i++;
		else {
			printf("dispatching from apto[%d]\n", i);
			dispatch(task);
		}
	}
	
	if ((task = dequeue(&bloqueado)) == NULL) {
		printf("All queues empty. Leaving. \n");
		return 0;
	}
	printf("dispatching from bloqueado\n");
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
	enqueue(tcb, &apto[prio]);

	/* cria novo contexto a partir do atual. */
	/* 'context' é só pra facilitar o acesso a tcb->context */
	getcontext(&tcb->context);
	context = &tcb->context;
	context->uc_link = &terminate_context;
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

	myield();
	return tcb->tid;
}



int mwait(int tid)
{
	int i, found = 0;
	TCB_t *ptr, *this;


	/* busca por tid nas threads existentes */
	i = 0;
	while (i < NUM_PRIO_LVLS && found == 0) {
		if ((ptr = search_queue(tid, apto[i])) != NULL) {
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
 
	/* faz thread tid chamar thread bloqueada ao seu término */
	ptr->context.uc_link = &executando->context;

	/* bloqueia thread em execução */
	executando->state = BLOQUEADO;
	this = executando;
	enqueue(this, &bloqueado);

	/* e chama o escalonador */
	swapcontext(&this->context, &sched_context);

	/* voltou, tira a thread do estado bloqueado */
	this->state = APTO;
	enqueue(this, &apto[this->tid]);
	swapcontext(&this->context, &sched_context);

	return 0;
}

int myield(void)
{
	executando->state = APTO;
	enqueue(executando, &apto[executando->tid]);
	swapcontext(&executando->context, &sched_context);
	return 0;
}

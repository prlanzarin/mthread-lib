#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/mdata.h"
#include "../include/mthread.h"
#define NUM_PRIO_LVLS 3
#define MAX_THREADS 64
#define UNLOCKED_MUTEX 0

enum state_t { CRIACAO, APTO, EXECUCAO, BLOQUEADO, TERMINO };

typedef struct waiting_TCB {
	int target_tid; /* Id da thread a qual este tcb está esperando */
	TCB_t *tcb; /* este tcb */
	struct waiting_TCB *next;
} wTCB_t;

TCB_t *apto[NUM_PRIO_LVLS] = {NULL, NULL, NULL};
TCB_t *executando = NULL;
wTCB_t *bloqueado = NULL;
TCB_t *bloqueado_mutex = NULL;
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

/* Bloqueia thread para aguardar por 'target_tid'. Se já há uma thread
 * aguardando por 'tid', retorna -1. Retorna 0 em caso de sucesso.
 */
int block_thread(TCB_t *tcb, int target_tid)
{
	wTCB_t *w_tcb, *ptr;

	ptr = bloqueado;
	/* verifica se há alguma thread aguardando por 'target_tid' */
	while (ptr != NULL) {
		if (ptr->target_tid == target_tid && target_tid != -1)
			return -1; /* já há alguém aguardando */
		ptr = ptr->next;
	}

	/* Bloqueia a thread */
	tcb->state = BLOQUEADO;
	w_tcb = malloc(sizeof(wTCB_t));
	w_tcb->target_tid = target_tid;
	w_tcb->tcb = tcb;

	if ((ptr = bloqueado) == NULL) {
	/* primeira tcb na lista de bloqueados */
		w_tcb->next = NULL;
		bloqueado = w_tcb;
		return 0;
	}
	/* adiciona ao início da lista */
	w_tcb->next = bloqueado;
	bloqueado = w_tcb;
	return 0;
}

/* desbloqueia thread ESPERANDO por 'tid' e a recoloca na fila de aptos */
int unblock_thread(int tid)
{
	wTCB_t *ptr, *prev;
	ucontext_t *context;

	ptr = bloqueado;
	if (ptr == NULL)
		return 0; /* não há threads bloqueadas */

	if (ptr->target_tid == tid) {
		ptr->tcb->state = APTO;
		enqueue(ptr->tcb, &apto[ptr->tcb->prio]);
		free(ptr);
		bloqueado = NULL;
		return 0;
	}

	prev = ptr;
	ptr = ptr->next;
	while (ptr != NULL) {
		if (ptr->target_tid == tid) {
			/* achou. Desbloqueia e chama o contexto da thread
			 * bloquada
			 */
			ptr->tcb->state = APTO;
			enqueue(ptr->tcb, &apto[ptr->tcb->prio]);
			prev->next = ptr->next; /* pode ser NULL */
			context = &ptr->tcb->context;
			free(ptr);
			setcontext(context);
		} else {
			prev = ptr;
			ptr = ptr->next;
		}
	}
	return -1; /* nenhuma thread estava aguardando por 'tid' */
}

void dummy_func() {};


/* Aloca espaço pra TCB da main thread. Inicializa o contexto do
 * escalonador e associa a função 'scheduler' à sua ativação.
 */
int initialize()
{
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

	executando = &main_tcb;
	return 0;
}


int dispatch(TCB_t *task)
{
	task->state = EXECUCAO;
	executando = task;
	setcontext(&task->context);
	return -1; /* algo deu errado */
}

void terminate()
{
	executando->state = TERMINO;
	free(executando->context.uc_stack.ss_sp);
	enqueue(executando, &termino);
	unblock_thread(executando->tid);
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
			dispatch(task);
		}
	}
	
	exit(0);
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
	if (tid >= tids)
		return -1; /* thread ainda não foi criada */
	if (search_queue(tid, termino) != NULL)
		return -1; /* já terminou, não precisa esperar */

	block_thread(executando, tid);

	/* e chama o escalonador */
	swapcontext(&executando->context, &sched_context);

	/* tudo certo, thread desbloqueada */
	return 0;
}

int myield(void)
{
	executando->state = APTO;
	enqueue(executando, &apto[executando->prio]);
	swapcontext(&executando->context, &sched_context);
	return 0;
}

int mmutex_init(mmutex_t *mtx)
{
	mtx->flag = UNLOCKED_MUTEX;
	mtx->first = NULL;
	mtx->last = NULL;
	return 0;
}

int mlock(mmutex_t *mtx)
{
	TCB_t *ptr;
	ptr = bloqueado_mutex;

	if(mtx->flag == UNLOCKED_MUTEX) {
		mtx->flag = 1;
		return 0;
	}
	else {
		/* tid code -1 é sinalização de espera por mutex */
		block_thread(executando, -1); 
		/* fila de threads trancados pelo mutex */
		enqueue(executando, &bloqueado_mutex);
		mtx->first = bloqueado_mutex;

		while(ptr->next != NULL)
			ptr = ptr->next;

		mtx->last = ptr;
	}
	return 0;
}

int munlock(mmutex_t *mtx)
{
	TCB_t *ptr;
	ptr = bloqueado_mutex;

	mtx->flag = UNLOCKED_MUTEX;
	
	if((unblock_thread(mtx->last->tid)) == -1)
		return -1;

	if((queue_remove(mtx->last->tid, &bloqueado_mutex)) == -1)
		return -1;

	if(ptr == NULL){
		mtx->first = NULL;
		mtx->last = NULL;
		return 0;
}
	while(ptr->next != NULL)
		ptr = ptr->next;

	mtx->last = ptr;

	return 0;
	
}

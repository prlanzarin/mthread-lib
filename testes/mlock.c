/*
 * mlock.c: teste da primitiva mlock. Consiste em criar 10 threads que executam
 * a função locker_room. A função locker_room imprime o TID da thread em questão
 * 20 vezes. Isso ocorre dentro de uma seção crítica: primeiro há  impressão
 * de seu TID por 10 vezes e a thread chama myield. Outras threads que tentarem
 * acessar a seção serão bloqueadas. Após voltar do yield, a thread imprime seu
 * tid mais 10 vezes.
 *
 */

#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>
#define MAX_THREADS 10

int tid[MAX_THREADS];
int counter[50];
mmutex_t mutex;

void *locker_room(void *arg){
	int i = 0;
	int tindex = *((int *) arg);

	printf("Thread %d está executando\n", tid[tindex]);

	mlock(&mutex);
	printf("Thread %d ENTRA na seção crítica.\n TID>> ", tid[tindex]);
	for(i = 0; i<10; i++)
		printf("%d", tid[tindex]);
	printf("\n");
	printf("Thread %d YIELDED\n", tid[tindex]);
	myield();
	printf("Thread %d VOLTA a seção crítica após yield.\n TID>> ", tid[tindex]);
	for(i = 0; i<10; i++)
		printf("%d", tid[tindex]);
	printf("\n");
	munlock(&mutex);
	printf("Thread %d SAI da seção crítica\n", tid[tindex]);
	free(arg);
	return;
}


int main() {
	int i = 0;
	int *index;

	if(mlock(&mutex) == 0)
		printf("A própria main se trancou; isso é possível!\n");
	munlock(&mutex);

	if(mmutex_init(&mutex) == 1){
		printf("Erro na inicialização do mutex.\n");
		exit(0);
}
	for(i = 0; i < MAX_THREADS; i++) {
		index = malloc(sizeof(int));
		*index = i;
		tid[i] = mcreate(1, locker_room, (void *) index);
		printf("Thread %d criada\n", tid[i]);
	}

	for(i = 0; i < MAX_THREADS; i++)
		mwait(tid[i]);

	exit(0);
}

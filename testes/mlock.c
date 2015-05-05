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
	printf("TINDEX %d \n", tindex);
	printf("Thread %d está executando\n", tid[tindex]);
	mlock(&mutex);

	printf("Thread %d entra na seção crítica e vai imprimir seu tid 10 vezes\n", tid[tindex]);
	for(i = 0; i<10; i++)
		printf("%d", tid[tindex]);
	printf("\n");
	printf("Thread %d vai usar yield dentro do lock\n", tid[tindex]);
	myield();
	printf("Thread %d volta na seção crítica e vai imprimir seu tid 10 vezes\n", tid[tindex]);
	for(i = 0; i<10; i++)
		printf("%d", tid[tindex]);
	printf("\n");
	munlock(&mutex);
	printf("Thread %d saindo da seção crítica\n", tid[tindex]);
	return;
}


int main() {
	int i = 0;

	/*if(mlock(&mutex) == 0)
		printf("A própria main se trancou; isso é possível!\n");
	munlock(&mutex);*/

	if(mmutex_init(&mutex) == 1){
		printf("Erro na inicialização do mutex.\n");
		exit(0);
}
	for(i = 0; i < MAX_THREADS; i++) {
		tid[i] = mcreate(1, locker_room, (void *) &i);
		printf("Thread %d criada\n", tid[i]);
	}

	for(i = 0; i < MAX_THREADS; i++)
		mwait(tid[i]);

	exit(0);
}

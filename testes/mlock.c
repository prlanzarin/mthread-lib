#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>
#define MAX_THREADS 10

int tid[MAX_THREADS];
int counter[50];
mmutex_t mutex;

void locker_room(void *arg){
	int i = 0;
	int thid = (int *)arg;

	printf("Thread %d está executando\n", thid);
	mlock(&mutex);

	printf("Thread %d entra na seção crítica e vai imprimir seu tid 10 vezes\n", thid);
	for(i = 0; i<10; i++)
		printf("%d", thid);
	printf("\n");
	printf("Thread %d vai usar yield dentro do lock\n", thid);
	myield();
	printf("Thread %d volta na seção crítica e vai imprimir seu tid 10 vezes\n", thid);
	for(i = 0; i<10; i++)
		printf("%d", thid);
	printf("\n");
	munlock(&mutex);
	printf("Thread %d saindo da seção crítica\n", thid);
}


int main() {
	int i = 0;
	
	if(mmutex_init(&mutex) == 1){
		printf("Erro na inicialização do mutex.\n");
		exit(0);
}
	for(i = 0; i < MAX_THREADS; i++) {
		tid[0] = mcreate(1, (void *) locker_room, (void *) tid[0]);
		printf("Thread %d criada\n", tid[0]);
	}

	for(i = 0; i < MAX_THREADS; i++)
		mwait(tid[i]);

	if(mlock(&mutex) == 0)
		printf("A própria main se trancou; isso é possível!\n");
	exit(0);
}

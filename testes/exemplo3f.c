
/*
 *	Programa de exemplo de uso da biblioteca sthread
 *
 *	Versão 1.0 - 25/03/2015
 *
 *	Sistemas Operacionais I - www.inf.ufrgs.br
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

mmutex_t mutex;

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	return;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}

void* func2(void *arg) {
        printf("Eu sou a thread ID2 imprimindo %d\n", *((int *)arg));
	return;
}


int main(int argc, char *argv[]) {

    int	id0, id1, id2;
	int i = 3;
	
    id0 = mcreate(0, func0, (void *)&i);
    id1 = mcreate(1, func1, (void *)&i);
    id2 = mcreate(1, func2, (void *)&i);
	printf("mutex init: %d\n", mmutex_init(&mutex));
    printf("Eu sou a main após a criação de ID0 e ID1\n");

    mwait(id0);
    mwait(id1);
    mwait(id2);
    printf("Eu sou a main voltando para terminar o programa\n");
}


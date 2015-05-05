/*
 * myield.c: teste da primitiva myield. Por se tratar de um escalonador não
 * preemptivo, myield é, ao lado do mwait, uma das maneiras de uma thread
 * deixar de executar, liberando espaço para que outras executem.
 *
 * Nos testes abaixo, tanto a thread que executa main como a thread que relativa
 * a func1 executam myield()'s alternados, permitindo interrupções demarcadas
 * pelos printf's. Nota-se que o programa não inclui mwait(). myield() permite
 * que todas funções imprimam suas saídas corretamente.
 *
 * A impressão da variável compartilhada 'x' mostra como as funções só executam
 * após a liberação da CPU pela chamada myield(). Assim, por exemplo, a thread 0
 * imprime 'x' após sua modificação pela main, e não com o valor que tinha
 * no momento de sua criação.
 */

#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>

void *func0(void *arg)
{
	printf("Sou a thread 0 imprimindo %d\n", *((int *) arg));
	return NULL;
}

void *func1(void *arg)
{
	printf("Sou a thread 1 imprimindo %d\n", *((int *) arg));
	myield();
	printf("Sou a thread 1, agora imprimindo %d\n", *((int *) arg));
	return NULL;
}


int main() {
	int x = 0;

	mcreate(0, func0, (void *) &x);
	x++;
	printf("Sou a main após criar a thread 0, com x = %d\n", x);
	myield();
	mcreate(0, func1, (void *) &x);
	x++;
	myield();
	x++;
	myield();
	

	exit(0);
}

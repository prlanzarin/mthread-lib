/*
 * mcreate.c: teste da primitiva mcreate. O teste em questão trata de analisar
 * os possíveis usos da primitiva mcreate. Uma série de threads são criadas pela
 * thread main: a thread 0, da função 0, explicita a funcionalidade da passagem
 * de parâmetros através do mcreate ao imprimir um valor x fixo (150). A thread
 * 1, por sua vez, executa a função 2 e visa demonstrar a habilidade de threads
 * comuns criar novas threads ao realizar a criação de uma nova thread 2, de
 * func 2, que imprime uma mensagem para demonstrar que está funcionando.
 * 
 * Na função main, também é testada a tentativa de criar uma thread com priori-
 * dade inválida, o que retorna uma mensagem da biblioteca destacando a natureza
 * do erro e a prioridade repassada na chamada da função.
 */

#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>

void *func0(void *arg)
{
	printf("Sou a thread 0 imprimindo %d\n", *((int *) arg));
	printf("Minha passagem de parâmetros funciona!.\n");
	return NULL;
}

void *func2(void *arg)
{
	printf("Sou a thread 2 e fui criada pela função 1.\n");
	return NULL;
}

void *func1(void *arg)
{
	int i;
	printf("Sou a thread 1 imprimindo %d\n", *((int *) arg));
	i = mcreate(0, func2, NULL);
	if(i != -1)
		printf("Sou a thread 1 e consegui criar uma nova thread!\n");
	else exit(0);
	mwait(i);
	return NULL;
}
int main() {
	int x = 150, i;

	i = mcreate(0, func0, (void *) &x);
	printf("Sou a main após criar a thread 0\n");
	mwait(i);

	i = mcreate(-1, func1, (void *) &x);
	if(i == -1){
		printf("Sou a main e não consegui criar uma nova thread!  ");
		printf("A prioridade que passei era inválida.\n");
	}
	else exit(0);

	i = mcreate(1, func1, (void *) &x);
	printf("Sou a main após criar a thread 1\n");
	mwait(i);

	exit(0);
}

/*
 * rc_adder.c: teste da primitiva mwait. Consiste na simulação de um 4 bit
 * ripple carry adder. O usuário deve entrar dois operando, A e B, do bit
 * mais significativo ao menos significativo. O teste consiste em criar
 * múltiplas threads, cada uma consistindo de um full adder, em sequência, para
 * que calculem a soma correspondente e passem o carry out para a thread
 * seguinte.
 */ 
#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_BITS 4

typedef struct AdderOps {
	bool a, b;
	bool cin, cout;
	bool s;
} FBitOp;

int tid[MAX_BITS];

FBitOp *full_adder(void *arg){
	bool ab;
	/* cálculo da soma e do carry out no full adder */
	FBitOp *ops = (FBitOp *) arg;
	ab = (ops->a) ^ (ops->b);
	ops->s = ops->cin ^ ab;
	ops->cout = (ops->cin & ab) | (ops->a & ops->b);

	return ops;
}


int main() {
	int i = 0;
	int a, b;
	FBitOp *sum[MAX_BITS];
	/* solicita ao usuário o primeiro operando A[3-0] */
	for(i = MAX_BITS-1; i >= 0; i--){
		printf("Insira A[%d]:", i);
		scanf("%d", &a);
		sum[i] = malloc(sizeof(FBitOp));
		sum[i]->a = (bool) a;
		sum[i]->cin = 0;
	}
	/* solicita ao usuário o segundo operando B[3-0] */
	for(i = MAX_BITS-1; i >= 0; i--){
		printf("Insira B[%d]:", i);
		scanf("%d", &b);
		sum[i]->b = (bool) b;
	}
	/* criação dos full adders e rapasse dos carry outs para o próximo */
	for(i = 0; i < MAX_BITS; i++) {
		tid[i] = mcreate(1, 
				(void *) full_adder, 
				(void *) sum[i]);
		mwait(tid[i]);
		/* repasse do carry out ao carry in do próximo full adder */
		if((i + 1) < MAX_BITS)
			sum[i+1]->cin = sum[i]->cout;
	}
	/* impressão do resultado */
	printf("Resultado: ");
	for(i = MAX_BITS - 1; i >= 0 ; i--)
		printf("%d", sum[i]->s);
	printf("\n");
	/*liberação das estruturas de full adder */
	for(i = MAX_BITS-1; i >= 0; i--)
		free(sum[i]);

	exit(0);
}

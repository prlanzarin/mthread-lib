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
	FBitOp *ops = (FBitOp *) arg;

	ab = (ops->a) ^ (ops->b);
	ops->s = ops->cin ^ ab;
	ops->cout = (ops->cin & ab) | (ops->a & ops->b);

	return ops;
}


int main() {
	int i = 0;
	size_t bits = 4;
	bool *a = NULL, *b = NULL;
	FBitOp *sum[MAX_BITS];

	printf("Insira A[3-0]: ");
	fflush(stdin);
	getline((char **)&a, &bits, stdin);

	printf("Insira B[3-0]: ");
	fflush(stdin);
	getline((char **)&b, &bits, stdin);

	for(i = MAX_BITS-1; i >= 0; i--){
		sum[i] = malloc(sizeof(FBitOp));
		sum[i]->a = (bool) a[i];
		sum[i]->b = (bool) b[i];
	}

	for(i = 0; i < MAX_BITS; i++) {
		tid[i] = mcreate(1, 
				(void *) full_adder, 
				(void *) sum[i]);
		mwait(tid[i]);
	}
	printf("Resultado: ");
	for(i = 0; i < MAX_BITS; i++)
		printf("%d", sum[i]->s);
	printf("\n");

	for(i = MAX_BITS-1; i >= 0; i--)
		free(sum[i]);
	free(a);
	free(b);
	exit(0);
}

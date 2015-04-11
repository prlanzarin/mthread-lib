/* 
 * test_vetor.c: realiza a criação de 10 threads, cada uma delas escreve uma
 * sequencia de 20 letras iguais e passa a vez para outra thread. Repete até
 * preencher um vetor de 250 caracteres.
 */

#include	"../include/mthread.h"
#include	<stdio.h>
#include	<stdlib.h>

#define		MAX_SIZE	250
#define		MAX_THR		10

int vetor[MAX_SIZE];
int  inc = 0;

mmutex_t mtx;

void *func(void *arg){
	mlock(&mtx);
	printf("START: LOCKED AREA\n");
	while ( inc < MAX_SIZE ) {
		vetor[inc] = (int)arg;
		inc++;
		if ( (inc % 20) == 0 ){
			printf("TASK YIELDED ON LOCK\n");
			myield(); 
		}
		else
			continue; 
	}
	munlock(&mtx);
	printf("END: LOCKED AREA. MUTEX = %d\n", mtx.flag);

   return (NULL);
}


int main(int argc, char *argv[]) {
    int i, pid[MAX_THR];

	mmutex_init(&mtx);
	for (i = 0; i < MAX_THR; i++) {
        	pid[i] = mcreate(1, func, (void *)('A'+i));
       		if ( pid[i] == -1) {
          		printf("ERRO: criação de thread!\n");
          		exit(-1);
       }
     }

    	for (i = 0; i < MAX_THR; i++) 
		mwait(pid[i]);

	for (i = 0; i < MAX_SIZE; i++) {    
		if ( (i % 20) == 0 )
			printf("\n");
			printf("%c", (char)vetor[i]);
	}
	printf("\nConcluido vetor de letras...\n");
	exit(0);
}


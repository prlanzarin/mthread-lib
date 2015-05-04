#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_BSIZE 8
#define MIN_BSIZE 1

typedef struct KnightPosition {
	int c;
	int r;
} KnightPos;

typedef struct KnightPositionList {
	KnightPos pos;
	struct KnightPositionList *next;
} KnightPosEl;

KnightPosEl *next_moves = NULL;
int calls = 0;

/* lista de posições -> inserção */
void insert_move(KnightPosEl *pos, KnightPosEl **queue)
{
	pos->next = *queue;
	*queue = pos;
}

/* determina todas as próximas posições que o cavalo pode atingir dada a
 * posição (c, r) de entrada
 */
KnightPosEl *moveKnight(void *arg)
{
	int c1, r1, r2, cpos, rpos;
	int v, h;
	KnightPosEl *ptr;
	KnightPos *pos_ptr;
	
	/* angaria os parâmetros passados pela estrutura de posições em arg*/
	pos_ptr = (KnightPos *)arg;
	cpos = pos_ptr->c;
	rpos = pos_ptr->r;

	/* calcula as próximas possíveis posições do cavalo a partir de 
	cpos e rpos */
	for(v = 2; v >= -2; v--) {
		switch(v) {
			case 2:
				c1 = cpos+v;
				r1 = rpos-1;
				r2 = rpos+1;
				break;
			case -2:
				c1 = cpos+v;
				r1 = rpos-1;
				r2 = rpos+1;
				break;
			case 1:
				c1 = cpos+v;
				r1 = rpos-2;
				r2 = rpos+2;

				break;
			case -1:
				c1 = cpos+v;
				r1 = rpos-2;
				r2 = rpos+2;
				break;

			default: break;
		}
	/* checa se são posições válidas perante o tabuleiro. Se forem, insere
	na lista de posições next_moves */
 		if(c1 <= 8 && c1 >= 1 && v != 0){
			if(r1 <= 8 && r1 >= 1) {
				ptr = malloc(sizeof(KnightPosEl));
				ptr->pos.c = c1;
				ptr->pos.r = r1;
				insert_move(ptr, &next_moves);
			}
			if(r2 <= 8 && r2 >= 1) {
				ptr = malloc(sizeof(KnightPosEl));
				ptr->pos.c = c1;
				ptr->pos.r = r2;
				insert_move(ptr, &next_moves);
			}
		}
	}
				
	return next_moves;
}

KnightPosEl *in3(void *arg){
	KnightPosEl *ptr;
	KnightPos *pos_ptr;
	
	pos_ptr = (KnightPos *)arg;
}
/*
bool canReachIn3(KnightPos org, KnightPos dst)
{
	int tid;
	KnightPos *first_moves;
	first_moves = moveKnight((void*) &org));
	calls++;
	ptr = next_moves;
	while(ptr != NULL) {
		tid[i] = mcreate(2, (void *) in3, (void *));
	}
}
*/
int main()
{
	KnightPosEl *moves, *ptr;
	KnightPos pos;
	pos.c = 6; pos.r = 2;
	moves = moveKnight((void*) &pos);
	ptr = moves;
	while(ptr != NULL) {
		printf("(%d, %d), ", ptr->pos.c, ptr->pos.r);
		ptr = ptr->next;
	}
	printf("\n");
	
	exit(0);
}

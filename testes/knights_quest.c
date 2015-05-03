#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>
#define BOARD_SIZE 8

typedef struct KnightPosition {
	int c;
	int r;
	struct KnightPosition *next;
} KnightPos;

void insert_move(KnightPos *pos, KnightPos **queue)
{
	pos->next = *queue;
	*queue = pos;
}

KnightPos *moveKnight(int c, int r)
{
	int c1, r1, r2;
	int v, h;
	KnightPos *next_moves  = NULL, *new_move;

	for(v = 2; v >= -2; v--) {
		switch(v) {
			case 2:
				c1 = c+v;
				r1 = r-1;
				r2 = r+1;
				break;
			case -2:
				c1 = c+v;
				r1 = r-1;
				r2 = r+1;
				break;
			case 1:
				c1 = c+v;
				r1 = r-2;
				r2 = r+2;
				break;
			case -1:
				c1 = c+v;
				r1 = r-2;
				r2 = r+2;
				break;

			default: break;
		}

 		if(c1 <= 8 && c1 >= 1 && v != 0){
			if(r1 <= 8 && r1 >= 1) {
				new_move = malloc(sizeof(KnightPos));
				new_move->c = c1;
				new_move->r = r1;
				insert_move(new_move, &next_moves);
			}
			if(r2 <= 8 && r2 >= 1) {
				new_move = malloc(sizeof(KnightPos));
				new_move->c = c1;
				new_move->r = r2;
				insert_move(new_move, &next_moves);
			}
			
		}
	}
				
	return next_moves;
}

int main()
{
	int c, r;
	KnightPos *moves, *ptr;
	moves = moveKnight(8,1);
	ptr = moves;
	while(ptr != NULL) {
		printf("(%d, %d), ", ptr->c, ptr->r);
		ptr = ptr->next;
	}
	printf("\n");
	
	return 0;
}

#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>

int mais_2(void *arg)
{
	int res;
	int x;
	x = *((int *) arg);
	res = x + 2;
	printf("%d mais_2: %d\n", x, res);
	return res;
}

int main()
{
	int x = 5;
	int tid;
	tid = mcreate(0, (void (*)(void *)) mais_2, (void *) &x);
	mwait(tid);
	return 0;
}

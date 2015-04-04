#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>

int mais_2(int x)
{
	int res;
	res = x + 2;
	printf("mais_2: %d\n", res);
	return res;
}

int main()
{
	int x = 5;
	int tid;
	tid = mcreate(0, (void (*)(void *)) mais_2, &x);
	mwait(tid);
	return 0;
}

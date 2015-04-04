#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/mdata.h"
#include "../include/mthread.h"
#define NUM_PRIO_LVLS 3

TCB *apto[NUM_PRIO_LVLS] = {NULL, NULL, NULL};
TCB *executando = NULL;
TCB *bloqueado = NULL;



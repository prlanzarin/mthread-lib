/* mmutex_init: testa o retorno de mmutex_init(). Retorna -1
 * caso a variável não foi alocada pelo usuário. Caso contrário, ela é
 * inicializada e a função retorna 0.
 */

#include "../include/mthread.h"
#include <stdio.h>
#include <stdlib.h>

mmutex_t *mutex;

int main() {
	int res = 0;
	res = mmutex_init(mutex);
	printf("Para mutex não alocado, mmutex_init retorna: %d\n", res);
	mutex = malloc(sizeof(mmutex_t));
	res = mmutex_init(mutex);
	printf("Ou %d caso contrário\n", res);

	exit(0);
}

/*
 * mandel.c: avalia o fractal de Mandelbrot dividindo o espaço de cálculo entre 'n' threads,
 *           onde 'n' é passado como parâmetro na linha de comando (Default = 1).
 *
 * Compile com: gcc -o mandel mandel.c -lpthread -lm -Wall
 *
 * execução:  mandel 3   (para 3 threads)
 */


#include	<sys/time.h>
#include	<stdio.h>
#include	"../include/mthread.h"
#include 	"../include/mdata.h"
#include	<stdlib.h>
#include	<string.h>
 
/* Definição de novos tipos de dados */
typedef	struct	{
	double	r, i;
}complex;

typedef struct work_t {
   int		ulx, uly, lrx, lry;
} wt;

/* Variáveis globais: compartilhada por todas as threads!! */

int		REGION_SIZE = 64;
int		IMAGE_SIZE = 1024;
int		MAX_ITERATIONS = 2000;

double		scale_x;
double		scale_y;
double		translate_x;
double		translate_y;

struct work_t* 	work;
char *          grid;
int		counter = 0;
mmutex_t mutex;

/* Fornece o temporizador no momento da chamada (em usec)*/
double	GetTime(void)
{
   struct  timeval time;
   double  Time;
   
   gettimeofday(&time, (struct timezone *) NULL);
   Time = ((double)time.tv_sec*1000000.0 + (double)time.tv_usec);
   return(Time);
}

/* Recupera uma região de cálculo de uma pilha de tarefas. Todas as
 * threads disponíveis disputam o acesso a uma tarefa.
 */

struct work_t* GetWork(void) {
    int aux;

    mlock(&mutex);
    counter = counter - 1;
    aux = counter;
    munlock(&mutex);

    if ( aux <= 0 )
       return NULL;
    else
       return (&work[aux]);
}
 
/* Converte as iterações necessárias a uma coordenada x,y convergir ou não
 * para valores RGB.
 */

char	xy2color(double x, double y)
{
    char            colour;
    int             i;
    complex         z, c;
    double          mag2;
    double          scale;

    scale = ((double) 255) / MAX_ITERATIONS;
    colour = -1;

    z.r = z.i = 0;
    c.r = x;
    c.i = y;
       
    for (i = 0; i < MAX_ITERATIONS; ++i) {

        x = (z.r * z.r) - (z.i * z.i) + c.r;
        y = (2 * z.r * z.i) + c.i;

        z.r = x;
        z.i = y;

        mag2 = (z.r * z.r) + (z.i * z.i);

        if (mag2 >= 4) {
           colour = (char) ((i * scale) + 0.5);
           break;
        }
    }

    return(colour);
}

/* Função executa pela THREAD. Recupera uma região de cálculo e avalia a convergência 
 * ou não de cada coordenada com o auxílio da função x2ycolor. O resultado é posto na
 * região global denominada de grid que será usada para o desenho final do fractal.
 */
 
void*	evaluate(void)
{
   int		ulx, uly, lrx, lry;
   int		i, j, region_size, len0, len1;
   char 	*region,*aux, *pgrid;
   struct work_t *WorkToDo;
  
   while (( WorkToDo = GetWork()) != NULL) {
      /* Apenas para simplificar a visualização do código, copia para variáveis locais */
     ulx = WorkToDo->ulx;
     uly = WorkToDo->uly;
     lrx = WorkToDo->lrx;
     lry = WorkToDo->lry;
 
     region_size = (lrx - ulx + 1) * (lry - uly +1);
     region = (char *) malloc( sizeof(char) * region_size);
   
     if (region == NULL) {
        printf("Memory allocation error on evaluate (region)...\n");
        exit(0);
     }
     
     aux = region;   
     for(i = uly ; i <= lry ; i++)
        for(j =ulx ; j <=lrx; j ++)
           *aux++=xy2color(translate_x + scale_x*j,
                           translate_y + scale_y*i);

     len0 = lrx - ulx + 1;
     len1 = lry - uly + 1;
     pgrid = grid + (IMAGE_SIZE * uly + ulx);
   
     aux = region;

     for (i = 0; i < len1; ++i) {
         memcpy(pgrid, aux, len0); 
         aux += len0;
         pgrid += IMAGE_SIZE;
     }
    free(region);
  } /* Final do while */
  return 0;
}  


int	main(int argc, char *argv[])
{
   double	grid_coord[4];
   double	t0, t1;
   int		i, n, nx, ny, x, y, nthr = 1, ids[8];
   FILE		*fp;
  
 /* Coordenadas do universo de cálculo */
   
   grid_coord[0] = -2.0;      /*x canto superior esquerdo */
   grid_coord[1] = 1.25;      /*y canto superior esquerdo */
   grid_coord[2] = 0.5;       /*x1 canto inferior direito  */
   grid_coord[3] = -1.25;     /*y1 canto inferior direito  */
 
 /* Inicialização das variáveis para determinar regiões */ 
   
   nx = IMAGE_SIZE/REGION_SIZE;
   ny = IMAGE_SIZE/REGION_SIZE;
   scale_x =  (grid_coord[2] - grid_coord[0]) / IMAGE_SIZE;
   scale_y = -((grid_coord[1] - grid_coord[3]) /IMAGE_SIZE);
   translate_x = grid_coord[0];
   translate_y = grid_coord[1];   

   grid = (char *)malloc(sizeof(char)*IMAGE_SIZE*IMAGE_SIZE);

   counter = nx*ny; 
   work = (struct work_t *)malloc(sizeof(struct work_t)*counter);

   if ( argc == 2 ) 
      nthr = atoi(argv[1]);

   if (work == NULL || grid == NULL) {
      printf("Memory allocation error...\n");
      exit(0);
   }

   /* Definição da unidade de trabalho: cada uma é uma região delimitada
    * pelas diagonais de coordenadas (ulx,uly) e (urx, ury).
    */

   i = 0;
   for(y = 0; y < IMAGE_SIZE; y += REGION_SIZE)
      for(x = 0; x < IMAGE_SIZE; x += REGION_SIZE) {
         work[i].ulx = x;
         work[i].uly = y;
         work[i].lrx = (x + REGION_SIZE > IMAGE_SIZE)? IMAGE_SIZE - 1 : x + REGION_SIZE - 1;
	 work[i].lry = (y + REGION_SIZE > IMAGE_SIZE)? IMAGE_SIZE - 1 : y + REGION_SIZE - 1;
         i++;
   }

   /* Efetua a criação de 'nthr' th reads para avaliar o fractal de Mandelbrot e espera até
    * que todas as threads não tenham mais trabalho para realizar.
    */

   printf("\nCalculando com %d threads... espere, por favor!\n", nthr);
   t0 = GetTime();
   for (i = 0; i < nthr; i++)
       ids[i] = mcreate(1, (void *)evaluate, NULL);

   for (i = 0; i < nthr; i++)
       mwait(ids[i]);
 
   t1 = GetTime();

   printf("--------------------------------------------\n");
   printf("Execution time for %2d threads: %.2f usecs\n", nthr, t1-t0);
   printf("--------------------------------------------\n");

 /* Geração de um arquivo ppm para a conferência visual de que o Fractal de Mandelbrot foi
  * calculado corretamente.
  */

   fp = fopen("mandel.ppm", "w");
   if (fp == NULL ) {
       printf("File open error...\n");
       exit(0);
   }
  
   fprintf(fp,"P3\n");
   fprintf(fp,"%d %d\n",IMAGE_SIZE, IMAGE_SIZE);
   fprintf(fp,"7\n");

   for(n=0; n < IMAGE_SIZE*IMAGE_SIZE; n++)
      fprintf(fp,"%d %d %d \n", (grid[n]>>5)&6, (grid[n]>>3)&7, (grid[n])&7);
		  
   fprintf(fp,"\n");
 
   fclose(fp);
   free(grid);
   free(work);
   exit(0);
}

/**
	* IF677 - Infraestrutura de Software 2016.1
  * Lista de threads EE1 - Questao 2
  * Bruno Filho(brgccf) e Pedro Rossi(pgrr)
	*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
/**
	* Exemplo de entrada:
	* // Ax = B
	* // N I, sendo N o numero de threads a serem usadas
	* // e I sendo o tamanho da matrix nXn
	* 2 2
	* // A matrix 'A'
	* 2 1
	* 5 7
	* // A matrix 'B'
	* 11
	* 13
	*/

// Define garantindo a quantidade de interacoes
#define P 100

/**
	* Estrutura auxiliar para passagem de argumentos
	*/
typedef struct Range {
	/**
		* Declaracao de variaveis da estrutura
		*/
	int start, end, k;
}Range;

/**
	* Declaracao de variaveis globais necessarias para o programa
	*/
int N, I, flag = 0;
double **A, *B, *C, *x;
Range *rg;
pthread_t *workers;
pthread_barrier_t barrier;

/**
	* Metodo calculante para cada xi
	*/
void* calc_xi(void* arg) {
	Range *aux = (Range*)arg;
	// 's' significa o x inicial para o qual a thread ira operar
	// 'f' significa o x final para o qual a thread ira operar
	// 'k' e uma variavel para contar as interacoes a cada x
	int s = aux->start, f = aux->end, k=aux->k;
	int i, j;
	// enquanto o numero de interacoes total nao foi atingido
	while(k<P) {
		for(i=s;i<=f;++i) {
			C[i] = B[i];
			// Operamos sobre a matrix auxiliar C (Parte do somatorio)
			for(j=0;j<I;++j) {
				if(i!=j) {
					C[i] = C[i] - A[i][j]*x[j];
				}
			}
		}
		// As threads esperam chegar aqui para nao alterarem nada
		// enquanto usamos x logo acima
		pthread_barrier_wait(&barrier);
		// Atribuimos o valor novo de x
		for(i=s;i<=f;++i) x[i] = C[i]/A[i][i];
		// As threads esperam a atribuição de x para nao voltar a
		// operar com x logo acima sem garantia de que ja foi alterado
		pthread_barrier_wait(&barrier);
		k++;
	}
	// As threads esperam as outras acabarem
	pthread_barrier_wait(&barrier);
	flag = 1;
	pthread_exit(NULL);
}

int main()
{
	// Variaveis para controle dos lacos
	int i, j;
	// Recebe N e I de entrada
	scanf("%d%d",&N,&I);
	// Alocacao do espaco necessario para A, B, C, X, workers e rg
	A = (double**)malloc(I*sizeof(double*));
	B = (double*)malloc(I*sizeof(double*));
	C = (double*)malloc(I*sizeof(double*));
	x = (double*)malloc(I*sizeof(double*));
	workers = (pthread_t*)malloc(N*sizeof(pthread_t));
	rg = (Range*)malloc(N*sizeof(Range));
	for(i=0;i<I;++i) A[i] = (double*)malloc(I*sizeof(double));
	// Atribuimos valores estaticos a x e C
	for(i=0;i<I;++i){
		x[i] = 1;
		C[i] = 0;
	}
	// Inicializamos a barrier
	pthread_barrier_init(&barrier, NULL, N);
	// Recebe A de entrada
	for(i=0;i<I;++i) {
	  for(j=0;j<I;++j) {
	    scanf("%lf",&A[i][j]);
		}
	}
	// Recebe B de entrada
	for(i=0;i<I;++i) {
		scanf("%lf",&B[i]);
	}
	// Aqui é feito a conta de quantos xi por thread teremos
	int jobs_per_thread, count=0;
	// Caso I nao seja multiplo de N dividir I/N retorna o
	// chao dessa operacao por isso é adicionado mais 1
	// caso nao, voce ja tem uma quantia certa de xi por threads
	if(I%N) jobs_per_thread = (I/N) + 1;
	else jobs_per_thread = (I/N);
	// com o auxilio de count e alocado xi para cada thread
	for(i=0;i<N;++i) {
		rg[i].start = count;
		if(count+jobs_per_thread<=I) count+=jobs_per_thread - 1;
		else count=I-1;
		rg[i].end = count;
		rg[i].k=0;
		++count;
	}
	// Iniciliza as threads
	for(i=0;i<N;++i) {
		pthread_create(&workers[i],NULL,calc_xi,(void*)&rg[i]);
	}
	// Join nas threads para impedir que elas acabem antes de main
	for(i=0;i<N;++i) {
		pthread_join(workers[i], NULL);
	}
	// Imprimi na tela o resultado
	for(i=0;i<I;++i) {
		printf("%lf\n", x[i]);
	}
	// Limpeza dos espaços alocados previamente
	for(i=0;i<I;++i) free(A[i]);
	free(A);
	free(B);
	free(C);
	free(x);
	free(workers);
	free(rg);
	// Limpeza da barrier
	pthread_barrier_destroy(&barrier);

	return 0;
}

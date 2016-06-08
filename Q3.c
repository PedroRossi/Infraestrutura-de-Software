/**
	* IF677 - Infraestrutura de Software 2016.1
  * Lista de threads EE1 - Questao 3
  * Bruno Filho(brgccf) e Pedro Rossi(pgrr)
	*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 100
#define BUFFER_SIZE 10

/**
	* Estrutura representando cada no da fila
	*/
typedef struct elem{
	/**
		* Declaracao de variaveis da estrutura elem
		*/
	int value;
	struct elem *prox;
}Elem;

/**
	* Estrutura representando a fila bloqueante
	*/
typedef struct blockingQueue{
	/**
		* Declaracao de variaveis da estrutura blockingQueue
		*/
	unsigned bufferSize, bufferStatus;
	Elem *head, *last;
}BlockingQueue;

/**
	* Declaracao de variaveis globais necessarias para o programa
	*/
pthread_t *productors;
pthread_t *consumers;
int flag_product = 1, flag_consume = 1;
pthread_mutex_t lock;
pthread_cond_t consuming, producing;

/**
	* Aloca o espaço de uma nova blockingQueue, seta
	* os parametros dela e a retorna.
	*/
BlockingQueue* newBlockingQueue(unsigned inBufferSize) {
	BlockingQueue* bq = (BlockingQueue*)malloc(sizeof(BlockingQueue));
	bq->bufferSize = inBufferSize;
	bq->bufferStatus = 0;
	bq->head = bq->last = NULL;
	return bq;
}

/**
	* Insere elementos no final da blockingQueue.
	*/
void putBlockingQueue(BlockingQueue* Q, int newValue) {
	if(Q->bufferSize == Q->bufferStatus) printf("Lista cheia!\n");
	// Cria um elem novo e atribui o value com val e o prox com NULL
	Elem *aux = (Elem*)malloc(sizeof(Elem));
	aux->value = newValue;
	aux->prox = NULL;
	// Trava o mutex
	pthread_mutex_lock(&lock);
	// Caso a fila esteja cheia espera o sinal de liberado
	while(Q->bufferSize == Q->bufferStatus) pthread_cond_wait(&producing, &lock);
	// Insere no ulimo o elem e aumenta o bufferStatus
	if(Q->head == NULL) {
		Q->head = Q->last = aux;
	} else {
		Q->last->prox = aux;
		Q->last = aux;
	}
	Q->bufferStatus++;
	// Sinaliza para quem for consumidor
	pthread_cond_signal(&consuming);
	// Destrava o mutex
	pthread_mutex_unlock(&lock);
	printf("Produziu: %d\n",newValue);
}

/**
	* Remove elementos do inicio da blockingQueue;
	*/
int takeBlockingQueue(BlockingQueue* Q) {
	if(!Q->bufferStatus) printf("Lista vazia!\n");
	// elem auxiliar
	Elem *aux;
	// Trava o mutex
	pthread_mutex_lock(&lock);
	// Caso a fila esteja vazia espera o sinal de liberado
	while(!Q->bufferStatus) pthread_cond_wait(&consuming, &lock);
	// aux recebe head, head aponta para o proximo dele e bufferStatus recebe ele menos 1
	aux = Q->head;
	Q->head = Q->head->prox;
	Q->bufferStatus--;
	// Sinaliza para quem for produtor
	pthread_cond_signal(&producing);
	// Destrava o mutex
	pthread_mutex_unlock(&lock);
	// Retorna o valor de aux e libera o espaco dele
	int ret = aux->value;
	free(aux);
	printf("Consumiu: %d\n",ret);
	return ret;
}

/**
	* Metodo produtor para a blockingQueue.
	*/
void* product(void* arg)
{
	BlockingQueue *a = (BlockingQueue*) arg;
	while(flag_product) {
		putBlockingQueue(a, rand()%100);
	}
}

/**
	* Metodo consumidor para a blockingQueue.
	*/
void* consume(void* arg)
{
	BlockingQueue *a = (BlockingQueue*) arg;
	while(flag_consume) {
		int aux = takeBlockingQueue(a);
	}
}

int main()
{
	// Seed aleatoria para randomizacao
	srand(time(NULL));
	// Inicializa mutex e conds
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&consuming, NULL);
	pthread_cond_init(&producing, NULL);
	// Cria nova BlockingQueue
	BlockingQueue *bq = newBlockingQueue(BUFFER_SIZE);
	// Alloca o espaco das Threads necessarias
	productors = (pthread_t*)malloc(NUM_THREADS*sizeof(pthread_t));
	consumers = (pthread_t*)malloc(NUM_THREADS*sizeof(pthread_t));
	// Initializacao dos consumidores e produtores
	int error, i;
	for(i=0;i<NUM_THREADS;++i) {
		error = pthread_create(&productors[i], NULL, product, (void*)bq);
		if(error) {
			printf("\nNao foi possivel criar a thread!\n");
		}
		error = pthread_create(&consumers[i], NULL, consume, (void*)bq);
		if(error) {
			printf("\nNao foi possivel criar a thread!\n");
		}
	}
	// Join para garantir que o main não finalize antes de alguma thread
	for(i=0;i<NUM_THREADS;++i) {
		pthread_join(consumers[i], NULL);
		pthread_join(productors[i], NULL);
	}
	// Destruicao dos mutexes e conds
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&producing);
	pthread_cond_destroy(&consuming);

	return 0;
}

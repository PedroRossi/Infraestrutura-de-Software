/**
	* IF677 - infraestrutura de Software 2016.1
  * Lista de threads EE1 - Questao 1
  * Bruno Filho(brgccf) e Pedro Rossi(pgrr)
	*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
	* Estrutura auxiliar para passagem de argumentos
	*/
typedef struct FileArg {
  /**
		* Declaracao de variaveis da estrutura
		*/
  FILE *file; // Ponteiro para arquivo
  int id; // id para saber qual mutex acessar na thread
}FileArg;

/**
	* Declaracao de variaveis globais necessarias para o programa
	*/
int T, N, F, *P, *custo_ruim, contador_bom, *workers_flag;
FILE **files;
FileArg **fa;
pthread_t *workers;
pthread_mutex_t *files_lock, contador_bom_lock;

/**
  * Metodo que opera sobre cada arquivo
  */
void* process(void* arg)
{
  // Recebe FileArg pelo argumento e atribui
  // os valores dele a id e file
  FileArg *aux = (FileArg*)arg;
  FILE *file = aux->file;
  int f, q, id = aux->id;
  // enquanto tiver f e q no arquivo
  // continuar lendo e operando
  while(fscanf(file, "%d%d", &f, &q)!=EOF) {
    if(q>=0) {
      // caso q > 0 travar a regiao e aumentar
      // o contador_bom em q flechas boas
      pthread_mutex_lock(&contador_bom_lock);
      contador_bom+=q;
      pthread_mutex_unlock(&contador_bom_lock);
    } else {
      // caso contrario travar a regiao e aumentar
      // o custo_ruim[i] na quantidade de flechas
      // a serem consertadas
      q *= -1;
      pthread_mutex_lock(&files_lock[id]);
      custo_ruim[f] += P[f]*q;
      pthread_mutex_unlock(&files_lock[id]);
    }
  }
  // o arquivo é fechado apos terminado de ler
  fclose(file);
  // a flag da thread atual e setada para 0
  pthread_mutex_lock(&files_lock[id]);
  workers_flag[id] = 0;
  pthread_mutex_unlock(&files_lock[id]);
  // a thread e encerrada
  pthread_exit(NULL);
}

int main()
{
  int i;
  // Entrada do usuario de T, N e F
  printf("Digite a quantia de threads: ");
  scanf("%d",&T);
  printf("Digite a quantia de arquivos: ");
  scanf("%d",&N);
  printf("Digite a quantia de flechas: ");
  scanf("%d",&F);
  // Alocacao dinamica do espaco necessario e
  // atribuicao dos valores necessarios aos mesmos
  workers = (pthread_t*)malloc(T*sizeof(pthread_t));
  files_lock = (pthread_mutex_t*)malloc(T*sizeof(pthread_mutex_t));
  workers_flag = (int*)malloc(T*sizeof(int));
  files = (FILE**)malloc(N*sizeof(FILE*));
  fa = (FileArg**)malloc(N*sizeof(FileArg*));
  P = (int*)malloc(F*sizeof(int));
  custo_ruim = (int*)malloc(F*sizeof(int));
  for(i=0;i<F;++i) P[i] = 0;
  for(i=0;i<F;++i) custo_ruim[i] = 0;
  for(i=0;i<T;++i) workers_flag[i] = 0;
  for(i=0;i<T;++i) pthread_mutex_init(&files_lock[i], NULL);
  pthread_mutex_init(&contador_bom_lock, NULL);
  contador_bom = 0;
  // Entrada do usuario do valor das flechas
  printf("Digite o valor das flechas: ");
  for(i=0;i<F;++i) scanf("%d",&P[i]);
  // Atribuicao dos ponteiros para cada arquivo necessario
  for(i=0;i<N;++i) {
    char aux[50];
    sprintf(aux,"%d",i+1);
    strcat(aux, ".in");
    files[i] = fopen(aux,"r");
    if(files[i] == NULL) {
      printf("Erro ao abrir o arquivo %d.in\n", i+1);
    }
  }
  // Enquanto tiver arquivos a serem lidos
  while(N) {
    for(i=0;i<T;++i) {
      // O primeiro worker com 0 (nao esta trabalhando)
      // e alocado para o ultimo arquivo e N é decrementado
      if(!workers_flag[i]) {
        // defini que a thread atual vai trabalhar
        // garantindo sua protecao por mutex
        pthread_mutex_lock(&files_lock[id]);
        workers_flag[i] = 1;
        pthread_mutex_unlock(&files_lock[id]);
        // e atribuido os valores de file e o id da thread atual
        // a uma estrutura auxiliar
        fa[i] = (FileArg*)malloc(sizeof(FileArg));
        fa[i]->file = files[N-1];
        fa[i]->id = i;
        // a thread e criada e logo apos dado join para nao finalizar antes do main
        pthread_create(&workers[i], NULL, process, (void*)fa[i]);
        pthread_join(workers[i],NULL);
        N--;
      }
      if(!N) break;
    }
  }
  // impressao do resultado na tela
  printf("\n%d flechas em bom estado!\n", contador_bom);
  for(i=0;i<F;++i) printf("Custo de consertar as flechas de tipo %d: R$ %.2f\n", i, (double)custo_ruim[i]);
  // destruicao dos mutexes
  for(i=0;i<T;++i) pthread_mutex_destroy(&files_lock[i]);
  // liberacao do espaço alocado previamente
  free(workers);
  free(workers_flag);
  for(i=0;i<N;++i) free(files[i]);
  for(i=0;i<N;++i) free(fa[i]);
  free(files);
  free(fa);
  free(P);

  return 0;
}

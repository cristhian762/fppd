#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

// IS - Insumo Secreto
// I  - Injeção
// VM - Viruas Atenuado

// LAB1 - IS1 I1
// LAB2 - VM1 TS2
// LAB3 - I2 VM2

// INF1 - IS
// INF2 - VM
// INF3 - I

typedef struct {
  int insumo[6];
} mesa;

typedef struct {
  int id; //  0   1   2     3    4    5
  mesa m; // IS1  I1  VM1  IS2   I2  VM2
} thread_arg;


void *thread(void *vargp);
void produzir(thread_arg *a);
void pegarInsumo(thread_arg *a);
void verificarLabs(thread_arg *a);

pthread_mutex_t mutex;

pthread_mutex_t mutex_cond[6];
pthread_cond_t cond[6];

void *thread(void *vargp){
  // Converte a estrutura recebida
  thread_arg *a = (thread_arg *) vargp;

  printf("Thread %d foi criado com sucesso\n", a->id );

  while(1) {
    produzir(a);
    pegarInsumo(a);
    verificarLabs(a);
  }

  pthread_exit((void *) NULL);
}

void produzir(thread_arg *a){
  if(a->id < 3){
    printf("\nLab %d: Se prepara para produzir o insumo.\n\n", a->id);
    pthread_cond_wait(&cond[a->id], &mutex_cond[a->id]);

    pthread_mutex_lock(&mutex_cond[a->id]);

    a->m.insumo[(a->id * 2)] = 1;
    a->m.insumo[(a->id * 2) + 1] = 1;

    pthread_mutex_unlock(&mutex_cond[a->id]);

    printf("\nLab %d: Insumo feito.\n\n", a->id);
  }
}

void pegarInsumo(thread_arg *a) {
  if(a->id > 2){
    int aux1 = 0, aux2 = 0;
    int encontrou = 0;
    
    printf("Inf %d: Se prepara para procurar na mesa.\n", a->id);
    
    if(a->id == 3) {
      pthread_mutex_lock (&(mutex));

      if(a->m.insumo[1] == 1) {
        aux1 = 1;
      } else if(a->m.insumo[4] == 1){
        aux1 = 4;
      }

      if(a->m.insumo[2] == 1) {
        aux2 = 2;
      } else if(a->m.insumo[5] == 1){
        aux2 = 5;
      }

      if(aux1 != 0 && aux2 != 0) {
        printf("Inf %d: Encontrou os insumos que precisava.\n", a->id);

        a->m.insumo[aux1] = 0;
        a->m.insumo[aux2] = 0;

        encontrou = 1;
      } else {
        printf("Inf %d: Não encontrou os insumos que precisava.\n", a->id);
      }

      pthread_mutex_unlock(&(mutex));
    }

    int r = (rand() % 10 + 1);

    if(encontrou){
      printf("Inf %d: Vai esperar %d segundos até vacinar novamente.\n", a->id, r);
    } else {
      printf("Inf %d: Vai esperar %d segundos para procurar novamente.\n", a->id, r);
    }

    sleep(r);
  }
}

void verificarLabs(thread_arg *a) {
  for (int i = 0; i < 3; i++) {
    if(a->m.insumo[(i * 2)] == 0 && a->m.insumo[(i * 2) + 1] == 0) {
      pthread_cond_signal(&cond[i]);
    }
  }
}

int main(int argc, char *argv[]) {
  pthread_t tid[6];
  thread_arg a[6];
  mesa m;

  int i = 0;
  int n_threads = 6;

  // Preenche a mesa
  for (i = 0; i < n_threads; i++){
    m.insumo[i] = 1;
  }

  // Cria o mutex
  pthread_mutex_init(&mutex, NULL);

  // Cria a variavel de condicao
  for (i = 0; i < n_threads; i++){
    pthread_cond_init(&cond[i],  NULL);
    pthread_mutex_init(&mutex_cond[i], NULL);
  }

  // Cria as threads
  for (i = 0; i < n_threads; i++){
    a[i].id = i;
    a[i].m = m;
    pthread_create(&tid[i], NULL, thread, (void *) &(a[i]));
  }

  // Espera que as threads terminem
  for (i = 0; i < n_threads; i++)
    pthread_join(tid[i], NULL);

  // Destroi o mutex
  pthread_mutex_destroy(&mutex);

  // Destroi variavel condicional
  for (i = 0; i < n_threads; i++){
    pthread_cond_destroy(&cond[i]);
    pthread_mutex_destroy(&mutex_cond[i]);
  }

  pthread_exit((void *) NULL);

  return 0;
}
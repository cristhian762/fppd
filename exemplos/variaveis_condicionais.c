#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
  int id;
} thread_arg;

void *thread(void *vargp);

pthread_mutex_t mutex;
pthread_cond_t cond;

int main(){
  pthread_t tid[2];
  thread_arg a[2];
  int i = 0;
  int n_threads = 2;

  // Cria o mutex
  pthread_mutex_init(&mutex, NULL);

  // Cria a variavel de condicao
  pthread_cond_init(&cond,  NULL);

  // Cria as threads
  for (i = 0; i < n_threads; i++){
    a[i].id = i;
    pthread_create(&tid[i], NULL, thread, (void *) &(a[i]));
  }

  // Espera que as threads terminem
  for (i = 0; i < n_threads; i++)
    pthread_join(tid[i], NULL);

  // Destroi o mutex
  pthread_mutex_destroy(&mutex);

  // Destroi variavel condicional
  pthread_cond_destroy(&cond);

  pthread_exit((void *) NULL);

  return 0;
}

void *thread(void *vargp){
  // Converte a estrutura recebida
  thread_arg *a = (thread_arg *) vargp;

  if(a->id == 0){
    printf("Thread %d: Esperando a outra thread\n", a->id);
    pthread_cond_wait(&cond, &mutex);
    printf("Thread %d: Acabou a espera\n", a->id);
  } else {
    sleep(3);
    printf("Thread %d: Sinalizando a outra thread\n", a->id);
    pthread_cond_signal(&cond);
  }

  pthread_exit((void *) NULL);
}
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int i;
  int j;
} thread_arg;

void *thread(void *vargp);

int main(){
  pthread_t tid;
  thread_arg a;

  a.i = 1;
  a.j = 2;

  pthread_create(&tid, NULL, thread, (void *) &a);
  pthread_join(tid, NULL);
  pthread_exit((void *) NULL);
}

void *thread(void *vargp){
  // Converte estrutura recebida
  thread_arg *a = (thread_arg *) vargp;
  int i = a->i;
  int j = a->j;

  printf("Parametros recebidos: %d %d\n", i, j);

  pthread_exit((void *) NULL);
}
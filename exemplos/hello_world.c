#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void *thread(void *vargp);

int main(){
  pthread_t tid;
  printf("Hello world da thread principal!\n");

  pthread_create(&tid, NULL, thread, NULL);
  pthread_join(tid, NULL);
  pthread_exit((void *) NULL);
}

void *thread(void *vargp){
  printf("Hello World da thread criada pela thread princial!\n");
  pthread_exit((void *) NULL);
}
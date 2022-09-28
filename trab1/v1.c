#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

// IS - Insumo Secreto
// SE - Seringa
// VA - Viruas Atenuado

// LAB1 - IS1 SE1
// LAB2 - VA1 TS2
// LAB3 - SE2 VA2

// INF1 - IS
// INF2 - VA
// INF3 - SE

typedef struct {
  char *insumo;
  int disponibilidade;
} m;

typedef struct {
  int id;
  char *nome;
  char *insumo;
  int vac_aplic;
} inf;

typedef struct {
  int id;
  char *prod1;
  char *prod2;
  int ins_prod;
} lab;

typedef struct {
  lab laboratorios[3];
  inf infectados[3];
  m mesa[6]; // IS1 SE1  VA1 IS2  SE2 VA2
} vac;

typedef struct {
  int id;
  vac vacinacao;
} thread_arg;

void *thread(void *vargp);
void preencher_vacinacao(vac v);

pthread_mutex_t mutex[6];
pthread_cond_t cond[6];

int main(){
  int n_threads = 90;
  int i = 0;
  pthread_t tid[n_threads];
  thread_arg th_a[n_threads];
  vac vacinacao;

  preencher_vacinacao(vacinacao);

  for (i = 0; i < 3; i++) {
    pthread_mutex_init(&mutex[i], NULL);
    pthread_cond_init(&cond[i], NULL);
  }

  for (i = 0; i < n_threads; i++){
    th_a[i].id = i;
    th_a[i].vacinacao = vacinacao;

    pthread_create(&tid[i], NULL, thread, (void *) &(th_a));
  }

  for (i = 0; i < n_threads; i++){
    pthread_join(tid[i], NULL);
  }

  for (i = 0; i < 3; i++) {
    pthread_mutex_destroy(&mutex[i]);
    pthread_cond_destroy(&cond[i]);
  }

  pthread_exit((void *) NULL);

  return 0;
}

void *thread(void *vargp){
  thread_arg *th_a = (thread_arg *) vargp;

  int pos = th_a->id % 6;
  printf("Thread: %d \n", th_a->id);
  printf("pos: %d \n", pos);
  printf("Mesa: %s \n\n", pos);

  // if(pos % 6 < 3) {
  //   if(th_a->vacinacao.mesa[pos * 2].disponibilidade == 1 && th_a->vacinacao.mesa[(pos * 2) + 1].disponibilidade == 1) {
  //     printf("LAB%d: Esperando para produzir\n", pos);
  //     pthread_cond_wait(&cond[pos], &mutex[pos]);
  //     pthread_mutex_lock(&mutex[pos]);

  //     th_a->vacinacao.mesa[pos * 2].disponibilidade = 1;
  //     th_a->vacinacao.mesa[(pos * 2) + 1].disponibilidade = 1;
      
  //     th_a->vacinacao.laboratorios[pos].ins_prod += 1;

  //     printf("LAB%d: Produziu insumo\n", pos);
  //     pthread_mutex_unlock(&mutex[pos]);
  //   }
  // }
}

void preencher_vacinacao(vac v){
  v.mesa[0].insumo = "IS";
  v.mesa[0].disponibilidade = 1;
  v.mesa[1].insumo = "SE";
  v.mesa[1].disponibilidade = 1;
  v.mesa[2].insumo = "VA";
  v.mesa[2].disponibilidade = 1;
  v.mesa[3].insumo = "IS";
  v.mesa[3].disponibilidade = 1;
  v.mesa[4].insumo = "SE";
  v.mesa[4].disponibilidade = 1;
  v.mesa[5].insumo = "VA";
  v.mesa[5].disponibilidade = 1;

  v.infectados[0].id = 1;
  v.infectados[0].nome = "João";
  v.infectados[0].insumo = "IS";
  v.infectados[0].vac_aplic = 0;

  v.infectados[1].id = 2;
  v.infectados[1].nome = "Maria";
  v.infectados[1].insumo = "VA";
  v.infectados[1].vac_aplic = 0;

  v.infectados[2].id = 3;
  v.infectados[2].nome = "Lucas";
  v.infectados[2].insumo = "SE";
  v.infectados[2].vac_aplic = 0;
  
  v.laboratorios[0].id = 1;
  v.laboratorios[0].prod1 = "IS";
  v.laboratorios[0].prod2 = "SE";
  v.laboratorios[0].ins_prod = 0;

  v.laboratorios[1].id = 2;
  v.laboratorios[1].prod1 = "VA";
  v.laboratorios[1].prod2 = "IS";
  v.laboratorios[1].ins_prod = 0;

  v.laboratorios[2].id = 3;
  v.laboratorios[2].prod1 = "SE";
  v.laboratorios[2].prod2 = "VA";
  v.laboratorios[2].ins_prod = 0;
}
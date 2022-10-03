// #####################################
// Autores: Cristhian Fontana Mattiuzzi e Ricardo Rocha Ribeiro
// #####################################

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
  int item_1[2];
  int item_2[2];
} infectados;

typedef struct {
  int insumo[6];
} mesa;

typedef struct {
  int lab_id;
  int inf_id;
  int contador[6];
  int cond_parada;
  int finish;
  mesa m;
  infectados inf[3];
  pthread_mutex_t mutex_mesa;
  pthread_mutex_t mutex_lab;
  pthread_mutex_t mutex_inf;
  pthread_mutex_t mutex_cond[6];
  pthread_cond_t cond[6];
} thread_arg;

void *thread_lab(void *vargp);
void *thread_inf(void *vargp);
int somaContador(thread_arg *vac);
int finish(thread_arg *vac);

int finish(thread_arg *vac) {
  for (int i = 0; i < 6; i++) {
    if(vac->contador[i] < vac->cond_parada) {
      return 1;
    }
  }

  return 0;
}

int somaContador(thread_arg *vac) {
  int soma = 0;

  for (int i = 0; i < 6; i++) {
    soma += vac->contador[i];
  }
  
  return soma;
}

void *thread_lab(void *vargp) {
  thread_arg *vac = (thread_arg *) vargp;

  int lab = 0;

  pthread_mutex_lock(&vac->mutex_lab);
  lab = vac->lab_id;
  vac->lab_id += 1;
  pthread_mutex_unlock(&vac->mutex_lab);

  while (vac->finish) {
    pthread_mutex_lock(&vac->mutex_mesa);
    if((vac->m.insumo[(lab * 2)] == 0) && (vac->m.insumo[(lab * 2) + 1] == 0) ){
      vac->m.insumo[(lab * 2)] = 1;
      vac->m.insumo[(lab * 2) + 1] = 1;
      vac->contador[lab] += 1;
    }
    pthread_mutex_unlock(&vac->mutex_mesa);

    // Faz o laboratório dormir
    pthread_mutex_lock(&vac->mutex_cond[lab]);
    pthread_cond_wait(&vac->cond[lab], &vac->mutex_cond[lab]);
    pthread_mutex_unlock(&vac->mutex_cond[lab]);
  }

  pthread_exit((void *) NULL);
}

void *thread_inf(void *vargp) {
  thread_arg *vac = (thread_arg *) vargp;

  int inf = 0, pos_item_1, pos_item_2;

  pthread_mutex_lock(&vac->mutex_inf);
  inf = vac->inf_id;
  vac->inf_id += 1;
  pthread_mutex_unlock(&vac->mutex_inf);

  while (vac->finish) {    
    pos_item_1 = -1;
    pos_item_2 = -1;

    pthread_mutex_lock(&vac->mutex_mesa);
    // Verifica se o isumo 1 está disponível
    if(vac->m.insumo[vac->inf[inf].item_1[0]] == 1){
      pos_item_1 = vac->inf[inf].item_1[0];
    } else if(vac->m.insumo[vac->inf[inf].item_1[1]] == 1){
      pos_item_1 = vac->inf[inf].item_1[1];
    }

    // Verifica se o isumo 2 está disponível
    if(vac->m.insumo[vac->inf[inf].item_2[0]] == 1){
      pos_item_2 = vac->inf[inf].item_2[0];
    } else if(vac->m.insumo[vac->inf[inf].item_2[1]] == 1){
      pos_item_2 = vac->inf[inf].item_2[1];
    }

    // Verifica se ambos estão disponível
    if(pos_item_1 != -1 && pos_item_2 != -1){
      vac->m.insumo[pos_item_1] = 0;
      vac->m.insumo[pos_item_2] = 0;
      vac->contador[(3 + inf)] += 1;
    }
    pthread_mutex_unlock(&vac->mutex_mesa);

    // Verifica se algum lab pode produzir acordar
    for (int i = 0; i < 3; i++) {
      if((vac->m.insumo[(i * 2)] == 0) && (vac->m.insumo[(i * 2) + 1] == 0) ){
          pthread_mutex_lock(&vac->mutex_cond[i]);
          pthread_cond_broadcast(&vac->cond[i]);
          pthread_mutex_unlock(&vac->mutex_cond[i]);
      }
    }
  }

  pthread_exit((void *) NULL);
}


int main(int argc, char *argv[]){
  int n_threads = 6, i = 0;

  pthread_t inf[3]; // Infectados
  pthread_t lab[3]; // Laboratorios
 
  thread_arg vac;

  // Iniciando vetor da mesa e contador
  for (i = 0; i < 6; i++) {
    vac.m.insumo[i] = 0;
    vac.contador[i] = 0;
  }
  // LAB0 - IS I
  // LAB1 - VM I
  // LAB2 - VM IS

  // INF0 - VM I
  // INF1 - IS I
  // INF2 - VM IS

  // Mesa - IS  I  VM   I   VM  IS  
  //        0   1  2    3   4   5

  // Definindo onde está o insumo necessário para cada infectado
  vac.inf[0].item_1[0] = 2;
  vac.inf[0].item_1[1] = 4;
  vac.inf[0].item_2[0] = 1;
  vac.inf[0].item_2[1] = 3;

  vac.inf[1].item_1[0] = 0;
  vac.inf[1].item_1[1] = 5;
  vac.inf[1].item_2[0] = 1;
  vac.inf[1].item_2[1] = 3;

  vac.inf[2].item_1[0] = 2;
  vac.inf[2].item_1[1] = 4;
  vac.inf[2].item_2[0] = 0;
  vac.inf[2].item_2[1] = 5;

  // Definindo condição de parada
  vac.cond_parada = strtol(argv[1], NULL, 10);

  vac.lab_id = 0;
  vac.inf_id = 0;
  vac.finish = 1;

  // Criando mutex
  pthread_mutex_init(&vac.mutex_lab, NULL);
  pthread_mutex_init(&vac.mutex_inf, NULL);
  pthread_mutex_init(&vac.mutex_mesa, NULL);

  // Criando as variaveisde condição
  for (i = 0; i < n_threads; i++){
    pthread_cond_init(&vac.cond[i],  NULL);
    pthread_mutex_init(&vac.mutex_cond[i], NULL);
  }

  // Cria as threads
  for (i = 0; i < 3; i++){
    pthread_create(&lab[i], NULL, thread_lab, (void *) &(vac));
    pthread_create(&inf[i], NULL, thread_inf, (void *) &(vac));
  }

  // Fica verificando para que todas as threads parem de uma vez
  int n = 0;
  while (vac.finish) {
    vac.finish = finish(&vac);
    if(n == somaContador(&vac)) {
      pthread_cond_broadcast(&vac.cond[3]);
      pthread_cond_broadcast(&vac.cond[4]);
      pthread_cond_broadcast(&vac.cond[5]);
    }

    n = somaContador(&vac);
  }

  for (int i = 0; i < 6; i++) {
    pthread_cond_broadcast(&vac.cond[i]);
  }

  // for (int i = 0; i < 3; i++) {
  //   pthread_join(lab[i], NULL);
  //   pthread_join(inf[i], NULL);
  // }

  printf("Laboratorio 1: %d\n", vac.contador[0]);
  printf("Laboratorio 2: %d\n", vac.contador[1]);
  printf("Laboratorio 3: %d\n", vac.contador[2]);
  printf("Infectado   1: %d\n", vac.contador[3]);
  printf("Infectado   2: %d\n", vac.contador[4]);
  printf("Infectado   3: %d\n", vac.contador[5]);
  
  // Destroi o mutex
  pthread_mutex_destroy(&vac.mutex_lab);
  pthread_mutex_destroy(&vac.mutex_inf);
  pthread_mutex_destroy(&vac.mutex_mesa);

  // Destroi variavel condicional
  for (i = 0; i < n_threads; i++){
    pthread_cond_destroy(&vac.cond[i]);
    pthread_mutex_destroy(&vac.mutex_cond[i]);
  }

  return 0;
}
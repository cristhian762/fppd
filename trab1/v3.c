// IS - Insumo Secreto
// I  - Injeção
// VM - Viruas Atenuado

// LAB1 - IS1 I1
// LAB2 - VM1 TS2
// LAB3 - I2 VM2

// INF1 - IS
// INF2 - VM
// INF3 - I

//         0   1   2     3    4    5
// Mesa - IS1  I1  VM1  IS2   I2  VM2

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
  int item_1[2]; // Posição a ser procurado na mesa
  int item_2[2]; // Posição a ser procurado na mesa
} infectados;

typedef struct {
  int pos[2]; // Posição do item a ser produzido
} laboratorios;

typedef struct {
  int insumo[6];
} mesa;

typedef struct {
  int contador[6];
  int cond_parada;
  int lab_id;
  int inf_id;
  mesa m;
  infectados inf[3];
  laboratorios lab[3];
  pthread_mutex_t mutex_mesa;
  pthread_mutex_t mutex_lab;
  pthread_mutex_t mutex_inf;
  pthread_mutex_t mutex_cond[6];
  pthread_cond_t cond[6];
} thread_arg;

void *thread_lab(void *vargp);
void *thread_inf(void *vargp);

void *thread_lab(void *vargp) {
  thread_arg *vac = (thread_arg *) vargp;

  while (vac->lab_id < 90)  {
    int lab = 0;
    lab = vac->lab_id % 3;
    pthread_mutex_lock(&vac->mutex_lab);
    vac->lab_id += 1;
    pthread_mutex_unlock(&vac->mutex_lab);

    printf("Laboratório %d está com a vez\n", lab);
    
    pthread_mutex_lock(&vac->mutex_cond[lab]);
    if((vac->m.insumo[(lab * 2)] == 1) && (vac->m.insumo[(lab * 2) + 1] == 1) ){
      pthread_cond_wait(&vac->cond[lab], &vac->mutex_cond[lab]);
    }
    pthread_mutex_unlock(&vac->mutex_cond[lab]);
    
    pthread_mutex_lock(&vac->mutex_mesa);
    vac->m.insumo[(lab * 2)] = 1;
    vac->m.insumo[(lab * 2) + 1] = 1;
    vac->contador[lab] += 1;
    pthread_mutex_unlock(&vac->mutex_mesa);
   
    printf("Laboratório %d produziu seus insumos.\n", lab);
    printf("Mesa: %d | %d | %d | %d | %d | %d\n", vac->m.insumo[0], vac->m.insumo[1], vac->m.insumo[2], vac->m.insumo[3], vac->m.insumo[4], vac->m.insumo[5]);
    printf("Contador: %d | %d | %d | %d | %d | %d\n", vac->contador[0], vac->contador[1], vac->contador[2], vac->contador[3], vac->contador[4], vac->contador[5]);

    // Verificar se algum infectado teve seu material disponibilizado por um lab
    // int pos_item_1 = 0, pos_item_2 = 0;
    // pthread_mutex_lock(&vac->mutex_cond[lab]);
    // if(vac->m.insumo[vac->inf[lab].item_1[0]] == 1){
    //   pos_item_1 = vac->inf[lab].item_1[0];
    // } else if(vac->m.insumo[vac->inf[lab].item_1[1]] == 1){
    //   pos_item_1 = vac->inf[lab].item_1[1];
    // }

    // if(vac->m.insumo[vac->inf[lab].item_2[0]] == 1){
    //   pos_item_2 = vac->inf[lab].item_2[0];
    // } else if(vac->m.insumo[vac->inf[lab].item_2[1]] == 1){
    //   pos_item_2 = vac->inf[lab].item_2[1];
    // }

    // if(pos_item_1 && pos_item_2) {
    //   pthread_cond_broadcast(&vac->cond[lab]);
    // } 
    // pthread_mutex_unlock(&vac->mutex_cond[lab]);
  
  
  }

  pthread_exit((void *) NULL);
}

void *thread_inf(void *vargp) {
  thread_arg *vac = (thread_arg *) vargp;

  while (vac->inf_id < 90)  {
    int pos_item_1 = 0, pos_item_2 = 0, inf = 0;
    inf = vac->inf_id % 3;
    pthread_mutex_lock(&vac->mutex_inf);
    vac->inf_id += 1;
    pthread_mutex_unlock(&vac->mutex_inf);

    pthread_mutex_lock(&vac->mutex_mesa);
    if(vac->m.insumo[vac->inf[inf].item_1[0]] == 1){
      pos_item_1 = vac->inf[inf].item_1[0];
    } else if(vac->m.insumo[vac->inf[inf].item_1[1]] == 1){
      pos_item_1 = vac->inf[inf].item_1[1];
    }

    if(vac->m.insumo[vac->inf[inf].item_2[0]] == 1){
      pos_item_2 = vac->inf[inf].item_2[0];
    } else if(vac->m.insumo[vac->inf[inf].item_2[1]] == 1){
      pos_item_2 = vac->inf[inf].item_2[1];
    }

    if(pos_item_1 && pos_item_2){
      vac->m.insumo[pos_item_1] = 0;
      vac->m.insumo[pos_item_2] = 0;
      vac->contador[(3 + inf)] += 1;

      printf("O infectado %d se vacinou.\n", inf);
    } else {
      printf("O infectado %d não se vacinou.\n", inf);
    }

    pthread_mutex_lock(&vac->mutex_cond[inf]);
    if((vac->m.insumo[(inf * 2)] == 1) && (vac->m.insumo[(inf * 2) + 1] == 1) ){
      pthread_cond_broadcast(&vac->cond[inf]);
    }
    pthread_mutex_unlock(&vac->mutex_cond[inf]);

    pthread_mutex_unlock(&vac->mutex_mesa);

    pthread_mutex_lock(&vac->mutex_cond[(3 + inf)]);
    pthread_cond_wait(&vac->cond[(3 + inf)], &vac->mutex_cond[(3 + inf)]);
    pthread_mutex_unlock(&vac->mutex_cond[(3 + inf)]);

    printf("Infectado criado\n");
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
  
  // Definindo onde está o insumo necessário para cada infectado
  vac.inf[0].item_1[0] = 1;
  vac.inf[0].item_1[1] = 4;
  vac.inf[0].item_2[0] = 2;
  vac.inf[0].item_2[1] = 5;

  vac.inf[1].item_1[0] = 0;
  vac.inf[1].item_1[1] = 3;
  vac.inf[1].item_2[0] = 1;
  vac.inf[1].item_2[1] = 4;

  vac.inf[2].item_1[0] = 0;
  vac.inf[2].item_1[1] = 3;
  vac.inf[2].item_2[0] = 2;
  vac.inf[2].item_2[1] = 5;

  // Definindo para onde vai os insumos produzidos pelos laboratórios
  vac.lab[0].pos[0] = 0;
  vac.lab[0].pos[1] = 1;
  vac.lab[1].pos[0] = 2;
  vac.lab[1].pos[1] = 3;
  vac.lab[2].pos[0] = 4;
  vac.lab[2].pos[1] = 5;

  // Definindo condição de parada
  vac.cond_parada = strtol(argv[1], NULL, 10);

  vac.lab_id = 0;
  vac.inf_id = 0;

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

  // Espera que as threads terminem
  for (i = 0; i < 3; i++){
    pthread_join(lab[i], NULL);
    pthread_join(inf[i], NULL);
  }

  // Destroi o mutex
  pthread_mutex_destroy(&vac.mutex_lab);
  pthread_mutex_destroy(&vac.mutex_inf);
  pthread_mutex_destroy(&vac.mutex_mesa);

  // Destroi variavel condicional
  for (i = 0; i < n_threads; i++){
    pthread_cond_destroy(&vac.cond[i]);
    pthread_mutex_destroy(&vac.mutex_cond[i]);
  }

  pthread_exit((void *) NULL);
  
  return 0;
}
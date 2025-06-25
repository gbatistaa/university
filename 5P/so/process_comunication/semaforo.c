#include <pthread.h> // Para criar e gerenciar threads
#include <stdio.h>   // Para printf
#include <unistd.h>  // Para usleep (espera em microssegundos)

#define NUM_ITER 5

// Definição da estrutura do semáforo
typedef struct {
  int value; // Valor atual do semáforo (normalmente 0 ou 1 para binário)
} Semaphore;

// Declaração global do semáforo
Semaphore sem;

// Função para inicializar o semáforo com valor inicial
void sem_init(Semaphore *s, int val) { s->value = val; }

// Operação down (wait/P) — tenta entrar na região crítica
void sem_down(Semaphore *s, int id) {
  while (1) {
    // Espera ocupada
    while (s->value <= 0) {
      // Busy wait
    }

    // Verifica se pode decrementar
    if (s->value > 0) {
      printf("Processo %d executando sem_down. Valor do semáforo antes: %d\n",
             id, s->value);
      s->value--; // entra na região crítica
      printf(
          "Processo %d entrou na região crítica. Valor do semáforo agora: %d\n",
          id, s->value);
      break;
    }
  }
}

// Operação up (signal/V) — libera a região crítica
void sem_up(Semaphore *s, int id) {
  s->value++; // libera o recurso
  printf("Processo %d executou sem_up. Valor do semáforo agora: %d\n", id,
         s->value);
}

// Função executada por cada thread/processo
void *process(void *arg) {
  int id = *(int *)arg;

  for (int i = 0; i < NUM_ITER; i++) {
    sem_down(&sem, id); // espera pelo semáforo

    // Região crítica
    printf(">>> Processo %d na região crítica. Iteração %d\n", id, i + 1);
    usleep(1000000); // 100 ms

    sem_up(&sem, id); // libera o semáforo

    // Região não crítica
    usleep(500000); // 50 ms
  }

  return NULL;
}

int main() {
  pthread_t t0, t1;
  int p0 = 0, p1 = 1;

  sem_init(&sem, 1); // semáforo começa liberado

  pthread_create(&t0, NULL, process, &p0);
  pthread_create(&t1, NULL, process, &p1);

  pthread_join(t0, NULL);
  pthread_join(t1, NULL);

  return 0;
}

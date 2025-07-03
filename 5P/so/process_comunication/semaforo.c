#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Estrutura do semáforo com valor atômico
typedef struct {
  atomic_int value;
} Semaphore;

typedef struct {
  int id;       // ID da thread
  int num_iter; // Número de iterações
} ThreadArgs;

Semaphore sem; // semáforo global

// Inicializa o semáforo
int sem_init(Semaphore *s, int val) {
  atomic_init(&s->value, val);
  return EXIT_SUCCESS;
}

// Operação down (P/wait) — espera até o semáforo estar disponível
int sem_down(Semaphore *s) {
  while (1) {
    // Tenta fazer fetch_sub se o valor for > 0
    int expected = atomic_load(&s->value);
    if (expected > 0) {
      if (atomic_compare_exchange_strong(&s->value, &expected, expected - 1)) {
        break; // conseguiu entrar na região crítica
      }
    }
    // Espera curta para evitar travamento de CPU (pode ser removido)
    usleep(100);
  }
  return EXIT_SUCCESS;
}

// Operação up (V/signal) — libera a região crítica
int sem_up(Semaphore *s) {
  atomic_fetch_add(&s->value, 1);
  return EXIT_SUCCESS;
}

// Região crítica simulada
atomic_int shared_counter = 0;

void *worker(void *arg) {
  ThreadArgs *args = (ThreadArgs *)arg; // Converte o ponteiro para a struct

  for (int i = 0; i < args->num_iter; i++) {
    sem_down(&sem);

    // Região crítica
    int local = atomic_load(&shared_counter);
    printf("Thread %d Increasing: %d → %d\n", args->id + 1, local, local + 1);
    atomic_store(&shared_counter, local + 1);

    usleep(100000); // simula trabalho

    sem_up(&sem);

    usleep(50000); // região não crítica
  }

  return NULL;
}

int main() {
  system("clear");
  pthread_t *threads;
  int num_threads, num_iter;
  long num_threads_available = sysconf(_SC_NPROCESSORS_ONLN);

  printf("Threads number: ");
  scanf("%d", &num_threads);

  if (num_threads > num_threads_available) {
    printf("Unfortunately I don't have %d threads, I will use my %lu threads\n",
           num_threads, num_threads_available);
    num_threads = (int)num_threads_available;
  }

  printf("Iterations per thread: ");
  scanf("%d", &num_iter);

  threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
  ThreadArgs *args =
      malloc(sizeof(ThreadArgs) * num_threads); // Aloca um array de structs

  sem_init(&sem, 1); // semáforo binário (exclusão mútua)

  for (int i = 0; i < num_threads; i++) {
    args[i].id = i;              // Define o ID da thread
    args[i].num_iter = num_iter; // Define o número de iterações
    pthread_create(&threads[i], NULL, worker,
                   &args[i]); // Passa o ponteiro para a struct
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Final value of the counter: %d\n", atomic_load(&shared_counter));

  free(threads); // Libera memória alocada
  free(args);    // Libera os argumentos

  return EXIT_SUCCESS;
}

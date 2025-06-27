#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_ITER 5

// Estrutura do semáforo com valor atômico
typedef struct {
  atomic_int value;
} Semaphore;

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
  int id = *(int *)arg;

  for (int i = 0; i < NUM_ITER; i++) {
    sem_down(&sem);

    // Região crítica
    int local = atomic_load(&shared_counter);
    printf("Thread %d Increasing: %d → %d\n", id, local, local + 1);
    atomic_store(&shared_counter, local + 1);

    usleep(100000); // simula trabalho

    sem_up(&sem);

    usleep(50000); // região não crítica
  }

  return NULL;
}

int main() {
  pthread_t *threads;
  int num_threads;
  long num_threads_available = sysconf(_SC_NPROCESSORS_ONLN);

  printf("Threads number: ");
  scanf("%d", &num_threads);

  if (num_threads > num_threads_available) {
    printf("Unfortunately I don't have %d threads, I will use my %lu threads",
           num_threads, num_threads_available);
    num_threads = (int)num_threads_available;
  }

  threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);

  sem_init(&sem, 1); // semáforo binário (exclusão mútua)

  for (int i = 0; i < num_threads; i++) {
    pthread_create(&threads[i], NULL, worker, &i);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Final value of the accountant: %d\n", atomic_load(&shared_counter));

  return EXIT_SUCCESS;
}

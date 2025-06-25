#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_ITER 5

volatile int interested[2] = {0, 0};
volatile int turn = 0;

void enterRegion(int process) {
  int other = 1 - process;
  interested[process] = 1;
  turn = process;
  while (interested[other] && turn == process) {
    // Espera ocupada (busy wait)
  }
}

void leaveRegion(int process) { interested[process] = 0; }

void *criticalSection(void *arg) {
  int process = *(int *)arg;
  for (int i = 0; i < NUM_ITER; i++) {
    enterRegion(process);

    // Região crítica
    printf("Processo %d na região crítica. Iteração %d\n", process, i + 1);

    double wait_time = 1;
    struct timespec req, rem;
    req.tv_sec = (time_t)wait_time;
    req.tv_nsec = (long)((wait_time - (double)req.tv_sec) * 1e9);
    nanosleep(&req, &rem);

    leaveRegion(process);

    // Região não crítica
    wait_time = 0.5;
    req.tv_sec = (time_t)wait_time;
    req.tv_nsec = (long)((wait_time - (double)req.tv_sec) * 1e9);
    nanosleep(&req, &rem);
  }
  return NULL;
}

int main() {
  pthread_t t0, t1;
  int p0 = 0, p1 = 1;

  pthread_create(&t0, NULL, criticalSection, &p0);
  pthread_create(&t1, NULL, criticalSection, &p1);

  pthread_join(t0, NULL);
  pthread_join(t1, NULL);

  return 0;
}

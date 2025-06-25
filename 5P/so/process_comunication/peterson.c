#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define NUM_ITER 5

volatile int interested[2] = {0, 0};
volatile int turn = 0;

void enterRegion(int process) {
  int other = 1 - process;
  interested[process] = 1;
  turn = process;
  while (interested[other] && turn == process) {
    // Busy wait
  }
}

void leaveRegion(int process) { interested[process] = 0; }

void *criticalSection(void *arg) {
  int process = *(int *)arg;
  struct timespec req;

  for (int i = 0; i < NUM_ITER; i++) {
    enterRegion(process);

    // Critical region
    printf("Process %d in the critical region. Iteration %d\n", process, i + 1);

    req.tv_sec = 1;
    req.tv_nsec = 0;
    nanosleep(&req, NULL);

    leaveRegion(process);

    // Non-critical region
    req.tv_sec = 0;
    req.tv_nsec = 500000000; // 0.5s
    nanosleep(&req, NULL);
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

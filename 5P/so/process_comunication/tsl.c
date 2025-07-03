#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_ITER 5

// Shared lock (0 = free, 1 = busy)
atomic_flag lock = ATOMIC_FLAG_INIT;

// Atomic Function of Lock Acquisition (TSL)
void acquire_lock() {
  while (atomic_flag_test_and_set(&lock)) {
    // Occupied waiting (Busy Wait)
  }
}

// Free or Lock
void release_lock() { atomic_flag_clear(&lock); }

// Shared critical region (simple example)
int shared_counter = 0;

void *worker(void *arg) {
  int id = *(int *)arg;

  for (int i = 0; i < NUM_ITER; i++) {
    acquire_lock(); // Entry into the critical region

    // Critical region protected by TSL
    printf("Thread %d increasing accountant (before: %d)\n", id,
           shared_counter);
    shared_counter++;
    printf("Thread %d increased to: %d\n", id, shared_counter);

    release_lock(); // EXIT OF THE CRITICAL REGION

    // Non -critical region
    usleep(1000000); // 1s
  }

  return NULL;
}

int main() {
  system("clear");
  pthread_t t0, t1;
  int id0 = 0, id1 = 1;

  pthread_create(&t0, NULL, worker, &id0);
  pthread_create(&t1, NULL, worker, &id1);

  pthread_join(t0, NULL);
  pthread_join(t1, NULL);

  printf("Final value of the shared counter: %d\n", shared_counter);

  return 0;
}

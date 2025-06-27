/* File:
 *    pth_hello.c
 *
 * Purpose:
 *    Illustrate basic use of pthreads:  create some threads,
 *    each of which prints a message.
 *
 * Input:
 *    none
 * Output:
 *    message from each thread
 *
 * Compile:  gcc -g -Wall -o pth_hello pth_hello.c -lpthread
 * Usage:    ./pth_hello <thread_count>
 *
 * IPP:   Section 4.2 (p. 153 and ff.)
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Global variable: accessible to all threads */
long thread_count = 0;

sem_t semaphore;

void *Hello(void *rank) {
  long my_rank = (long)rank;
  thread_count--;
  printf("Hello from thread %ld of %ld\n", my_rank, thread_count);
  sleep(2);
  printf("Thread %ld of %ld exit!\n", my_rank, thread_count);
  sem_post(&semaphore);
  return NULL;
}

/*--------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
  sem_init(&semaphore, 0, 3);
  long thread; /* Use long in case of a 64-bit system */

  thread_count = sysconf(_SC_NPROCESSORS_ONLN);

  pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));

  for (thread = 0; thread < thread_count; thread++) {
    // Coloca-se antes porque ele trava a criação de mais threads
    // Garantindo que somente a quantidade desejada de threads seja criada:
    sem_wait(&semaphore);
    pthread_create(&thread_handles[thread], NULL, Hello, (void *)thread);
  }

  printf("Hello from the main thread\n");
  usleep(100000);
  for (thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL); // Bloqueante
    printf("Thread %ld joined!\n", thread);
  }

  free(thread_handles);
  printf("Main thread exit\n");

  sem_destroy(&semaphore);
  return 0;
}

/*-------------------------------------------------------------------*/

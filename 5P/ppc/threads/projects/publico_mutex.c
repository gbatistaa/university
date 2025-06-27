#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

int total_public = 0;
pthread_mutex_t mutex;

int increment_public(int public) {
  for (int i = 0; i < public; i++) {
    total_public++;
  }
  return EXIT_SUCCESS;
}

void *routine(void *arg) {
  int *public = (int *)arg;

  pthread_mutex_lock(&mutex);
  increment_public(*public);
  pthread_mutex_unlock(&mutex);

  return NULL;
}

int main() {
  pthread_mutex_init(&mutex, NULL);

  int people_per_entry, entries_num;

  printf("People per entry: ");
  scanf("%d", &people_per_entry);
  printf("Entries number: ");
  scanf("%d", &entries_num);

  pthread_t *threads = malloc(sizeof(pthread_t) * entries_num);
  int *ids = malloc(sizeof(int) * entries_num);

  for (int i = 0; i < entries_num; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, routine, &people_per_entry);
  }

  for (int i = 0; i < entries_num; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Total public is: %d\n", total_public);

  return EXIT_SUCCESS;
}

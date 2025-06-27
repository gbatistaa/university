#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_ITER 5

volatile int interested[2] = {0, 0}; // Flags de interesse de cada processo
volatile int turn = 0;               // Indica de quem é a vez

void enter_region(int process) {
  int other = 1 - process;
  interested[process] = 1; // Este processo quer entrar
  turn = other;            // Dá a vez ao outro
  while (interested[other] && turn == other) {
    // Espera ocupada enquanto o outro quiser entrar e for a vez dele
  }
}

void leave_region(int process) {
  interested[process] = 0; // Sai da região crítica
}

void *critical_section(void *arg) {
  int process = *(int *)arg;
  for (int i = 0; i < NUM_ITER; i++) {
    enter_region(process);

    // Região crítica
    printf("Processo %d na região crítica. Iteração %d\n", process, i + 1);
    usleep(100000); // 100 ms

    leave_region(process);

    // Região não crítica
    usleep(50000);
  }

  return NULL;
}

int main() {
  pthread_t t0, t1;
  int p0 = 0, p1 = 1;

  pthread_create(&t0, NULL, critical_section, &p0);
  pthread_create(&t1, NULL, critical_section, &p1);

  pthread_join(t0, NULL);
  pthread_join(t1, NULL);

  return 0;
}

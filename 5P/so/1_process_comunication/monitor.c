#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Monitor: estrutura que encapsula dados e sincronização
typedef struct {
  int valor;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} ContadorMonitor;

// Inicializa o monitor
void monitor_init(ContadorMonitor *m) {
  m->valor = 0;
  pthread_mutex_init(&m->mutex, NULL);
  pthread_cond_init(&m->cond, NULL);
}

// Incrementa valor com exclusão mútua
void monitor_incrementar(ContadorMonitor *m) {
  pthread_mutex_lock(&m->mutex);

  m->valor++;
  printf("[Produtor] Valor = %d\n", m->valor);

  if (m->valor >= 10) {
    pthread_cond_broadcast(&m->cond); // Acorda todas as threads esperando
  }

  pthread_mutex_unlock(&m->mutex);
}

// Espera até o valor atingir um mínimo
void monitor_esperar_ate(ContadorMonitor *m, int minimo) {
  pthread_mutex_lock(&m->mutex);

  while (m->valor < minimo) {
    printf("[Consumidor] Aguardando valor >= %d...\n", minimo);
    pthread_cond_wait(&m->cond, &m->mutex);
  }

  printf("[Consumidor] Alcançado valor >= %d!\n", minimo);
  pthread_mutex_unlock(&m->mutex);
}

ContadorMonitor monitor;

void *produtor(void *arg) {
  for (int i = 0; i < 15; i++) {
    monitor_incrementar(&monitor);
    usleep(100000); // 100 ms
  }
  return NULL;
}

void *consumidor(void *arg) {
  monitor_esperar_ate(&monitor, 10);
  printf("[Consumidor] Continuando execução...\n");
  return NULL;
}

int main() {
  system("clear");
  monitor_init(&monitor);

  pthread_t t_prod, t_cons;
  pthread_create(&t_prod, NULL, produtor, NULL);
  pthread_create(&t_cons, NULL, consumidor, NULL);

  pthread_join(t_prod, NULL);
  pthread_join(t_cons, NULL);

  return 0;
}

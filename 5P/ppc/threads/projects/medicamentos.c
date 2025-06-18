#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PSEUDO_SECOND 0.2

typedef struct medicacao {
  char nome[50];
  int intervalo;
  int total;
} Medicamento;

void *Dorflex(void *param);
void *Paracetamol(void *param);
void *B12(void *param);

int main() {
  pthread_t dorflex_thread;
  pthread_t paracetamol_thread;
  pthread_t b12_thread;

  Medicamento *dorflex = (Medicamento *)malloc(sizeof(Medicamento));
  strcpy(dorflex->nome, "dorflex");
  dorflex->intervalo = 6;
  dorflex->total = 10;

  Medicamento *paracetamol = (Medicamento *)malloc(sizeof(Medicamento));
  strcpy(paracetamol->nome, "paracetamol");
  paracetamol->intervalo = 8;
  paracetamol->total = 12;

  Medicamento *b12 = (Medicamento *)malloc(sizeof(Medicamento));
  strcpy(b12->nome, "b12");
  b12->intervalo = 24;
  b12->total = 6;

  pthread_create(&dorflex_thread, NULL, Dorflex, (Medicamento *)dorflex);
  pthread_create(&paracetamol_thread, NULL, Paracetamol,
                 (Medicamento *)paracetamol);
  pthread_create(&b12_thread, NULL, B12, (Medicamento *)b12);

  pthread_join(dorflex_thread, NULL);
  pthread_join(paracetamol_thread, NULL);
  pthread_join(b12_thread, NULL);

  return EXIT_SUCCESS;
}

void *Dorflex(void *medicamento) {
  Medicamento *dorflex = (Medicamento *)medicamento;

  for (int i = 1; i <= dorflex->total; i++) {
    printf("Lembrete: tomar %s | Pílulas restantes %d\n", dorflex->nome, dorflex->total - i);
    sleep(dorflex->intervalo * PSEUDO_SECOND);
  }

  return NULL;
}

void *Paracetamol(void *medicamento) {
  Medicamento *paracetamol = (Medicamento *)medicamento;

  for (int i = 1; i <= paracetamol->total; i++) {
    printf("Lembrete: tomar %s | Pílulas restantes %d\n", paracetamol->nome, paracetamol->total - i);
    sleep(paracetamol->intervalo * PSEUDO_SECOND);
  }

  return NULL;
}

void *B12(void *medicamento) {
  Medicamento *b12 = (Medicamento *)medicamento;

  for (int i = 1; i <= b12->total; i++) {
    printf("Lembrete: tomar %s | Pílulas restantes: %d\n", b12->nome, b12->total - i);
    sleep(b12->intervalo * PSEUDO_SECOND);
  }

  return NULL;
}

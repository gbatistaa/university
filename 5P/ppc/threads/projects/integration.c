#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define VECTOR_SIZE 3
#define BUFFER_SIZE 5
#define MSG_EVENTO 1
#define MSG_TERMINO 0

typedef struct {
  int relogio[VECTOR_SIZE];
  int origem;
  int tipo;
} Mensagem;

Mensagem filaEntrada[BUFFER_SIZE];
int inEntrada = 0, outEntrada = 0, countEntrada = 0;
pthread_mutex_t mutexEntrada = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condEntradaCheia = PTHREAD_COND_INITIALIZER;
pthread_cond_t condEntradaVazia = PTHREAD_COND_INITIALIZER;

Mensagem filaSaida[BUFFER_SIZE];
int inSaida = 0, outSaida = 0, countSaida = 0;
pthread_mutex_t mutexSaida = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condSaidaCheia = PTHREAD_COND_INITIALIZER;
pthread_cond_t condSaidaVazia = PTHREAD_COND_INITIALIZER;

int comm_size, my_rank;

void *thread_entrada(void *arg) {
  while (1) {
    Mensagem msg;
    MPI_Status status;
    MPI_Recv(msg.relogio, VECTOR_SIZE, MPI_INT, MPI_ANY_SOURCE, 0,
             MPI_COMM_WORLD, &status);

    if (status.MPI_SOURCE == my_rank) {
      msg.tipo = MSG_TERMINO;
    } else {
      msg.tipo = MSG_EVENTO;
    }
    msg.origem = status.MPI_SOURCE;

    pthread_mutex_lock(&mutexEntrada);
    while (countEntrada == BUFFER_SIZE) {
      pthread_cond_wait(&condEntradaCheia, &mutexEntrada);
    }
    filaEntrada[inEntrada] = msg;
    inEntrada = (inEntrada + 1) % BUFFER_SIZE;
    countEntrada++;
    pthread_cond_signal(&condEntradaVazia);
    pthread_mutex_unlock(&mutexEntrada);

    if (msg.tipo == MSG_TERMINO)
      break;
  }
  return NULL;
}

void *thread_saida(void *arg) {
  while (1) {
    Mensagem msg_para_enviar;
    pthread_mutex_lock(&mutexSaida);
    while (countSaida == 0) {
      pthread_cond_wait(&condSaidaVazia, &mutexSaida);
    }
    msg_para_enviar = filaSaida[outSaida];
    outSaida = (outSaida + 1) % BUFFER_SIZE;
    countSaida--;
    pthread_cond_signal(&condSaidaCheia);
    pthread_mutex_unlock(&mutexSaida);

    if (msg_para_enviar.tipo == MSG_TERMINO)
      break;

    int destino = msg_para_enviar.origem;
    MPI_Send(msg_para_enviar.relogio, VECTOR_SIZE, MPI_INT, destino, 0,
             MPI_COMM_WORLD);
  }
  return NULL;
}

void imprimir_vetor(int *vetor, int rank, const char *evento_desc) {
  printf("[P%d] %s. Relógio: [%d,%d,%d]\n", rank, evento_desc, vetor[0],
         vetor[1], vetor[2]);
}

void evento_interno(int *vetor_local, const char *nome_evento) {
  vetor_local[my_rank]++;
  char buffer[50];
  sprintf(buffer, "Evento interno '%s'", nome_evento);
  imprimir_vetor(vetor_local, my_rank, buffer);
}

void preparar_envio(int *vetor_local, int destino, const char *nome_evento) {
  vetor_local[my_rank]++;
  char buffer[50];
  sprintf(buffer, "Evento '%s', enviando para P%d", nome_evento, destino);
  imprimir_vetor(vetor_local, my_rank, buffer);

  pthread_mutex_lock(&mutexSaida);
  while (countSaida == BUFFER_SIZE) {
    pthread_cond_wait(&condSaidaCheia, &mutexSaida);
  }
  filaSaida[inSaida].tipo = MSG_EVENTO;
  filaSaida[inSaida].origem = destino;
  for (int i = 0; i < VECTOR_SIZE; i++)
    filaSaida[inSaida].relogio[i] = vetor_local[i];
  inSaida = (inSaida + 1) % BUFFER_SIZE;
  countSaida++;
  pthread_cond_signal(&condSaidaVazia);
  pthread_mutex_unlock(&mutexSaida);
}

void esperar_recebimento(int *vetor_local, const char *nome_evento) {
  Mensagem msg_recebida;
  pthread_mutex_lock(&mutexEntrada);
  while (countEntrada == 0) {
    pthread_cond_wait(&condEntradaVazia, &mutexEntrada);
  }
  msg_recebida = filaEntrada[outEntrada];
  outEntrada = (outEntrada + 1) % BUFFER_SIZE;
  countEntrada--;
  pthread_cond_signal(&condEntradaCheia);
  pthread_mutex_unlock(&mutexEntrada);

  for (int i = 0; i < VECTOR_SIZE; i++) {
    if (msg_recebida.relogio[i] > vetor_local[i]) {
      vetor_local[i] = msg_recebida.relogio[i];
    }
  }
  vetor_local[my_rank]++;
  char buffer[50];
  sprintf(buffer, "Evento '%s', recebido de P%d", nome_evento,
          msg_recebida.origem);
  imprimir_vetor(vetor_local, my_rank, buffer);
}

void *thread_logica_programa(void *arg) {
  int vetor_local[VECTOR_SIZE] = {0};
  MPI_Barrier(MPI_COMM_WORLD);

  switch (my_rank) {
  case 0:
    evento_interno(vetor_local, "a");
    preparar_envio(vetor_local, 1, "b");
    esperar_recebimento(vetor_local, "c");
    preparar_envio(vetor_local, 2, "d");
    esperar_recebimento(vetor_local, "e");
    preparar_envio(vetor_local, 1, "f");
    evento_interno(vetor_local, "g");
    break;

  case 1:
    preparar_envio(vetor_local, 0, "h");
    esperar_recebimento(vetor_local, "i");
    esperar_recebimento(vetor_local, "j");

  case 2:
    evento_interno(vetor_local, "k");
    preparar_envio(vetor_local, 0, "l");
    break;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  sleep(1);

  MPI_Send(vetor_local, VECTOR_SIZE, MPI_INT, my_rank, 0, MPI_COMM_WORLD);

  pthread_mutex_lock(&mutexSaida);
  filaSaida[inSaida].tipo = MSG_TERMINO;
  inSaida = (inSaida + 1) % BUFFER_SIZE;
  countSaida++;
  pthread_cond_signal(&condSaidaVazia);
  pthread_mutex_unlock(&mutexSaida);

  return NULL;
}

int main(int argc, char *argv[]) {
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  if (provided < MPI_THREAD_MULTIPLE) {
    fprintf(stderr,
            "Erro: O ambiente MPI não oferece suporte a múltiplas threads.\n");
    MPI_Finalize();
    return 1;
  }

  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (VECTOR_SIZE != comm_size) {
    if (my_rank == 0) {
      fprintf(stderr,
              "Erro: VECTOR_SIZE (%d) deve ser igual ao número de processos "
              "MPI (%d).\n",
              VECTOR_SIZE, comm_size);
    }
    MPI_Finalize();
    return 1;
  }

  pthread_t entrada, logica, saida;

  pthread_create(&entrada, NULL, thread_entrada, NULL);
  pthread_create(&logica, NULL, thread_logica_programa, NULL);
  pthread_create(&saida, NULL, thread_saida, NULL);

  pthread_join(entrada, NULL);
  pthread_join(logica, NULL);
  pthread_join(saida, NULL);

  MPI_Finalize();
  return 0;
}

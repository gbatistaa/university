#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VECTOR_SIZE 3
#define BUFFER_SIZE 5
#define MSG_EVENTO 1
#define MSG_MARCADOR 2
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

pthread_mutex_t snapshot_mutex = PTHREAD_MUTEX_INITIALIZER;
int snapshot_ativo = 0;
int estado_local_snapshot[VECTOR_SIZE];
int marcador_recebido[VECTOR_SIZE] = {0};
Mensagem mensagens_canal[VECTOR_SIZE][BUFFER_SIZE];
int count_mensagens_canal[VECTOR_SIZE] = {0};
const int relogio_marcador[VECTOR_SIZE] = {-1, -1, -1};

void imprimir_vetor(int *vetor, int rank, const char *evento_desc) {
  printf("[P%d] %s. Relógio: [%d,%d,%d]\n", rank, evento_desc, vetor[0],
         vetor[1], vetor[2]);
}

void imprimir_snapshot_final() {
  printf("\n================ SNAPSHOT FINALIZADO PARA P%d ================\n",
         my_rank);
  printf("[P%d] Estado local salvo: [%d,%d,%d]\n", my_rank,
         estado_local_snapshot[0], estado_local_snapshot[1],
         estado_local_snapshot[2]);
  for (int i = 0; i < comm_size; i++) {
    if (i == my_rank)
      continue;
    printf("[P%d] Mensagens recebidas no canal %d->%d: %d\n", my_rank, i,
           my_rank, count_mensagens_canal[i]);
    for (int j = 0; j < count_mensagens_canal[i]; j++) {
      printf("      -> Msg de P%d: [%d,%d,%d]\n", mensagens_canal[i][j].origem,
             mensagens_canal[i][j].relogio[0], mensagens_canal[i][j].relogio[1],
             mensagens_canal[i][j].relogio[2]);
    }
  }
  printf("=============================================================\n\n");

  snapshot_ativo = 0;
}

void processar_marcador(int *vetor_local, int origem) {
  pthread_mutex_lock(&snapshot_mutex);

  if (marcador_recebido[origem]) {
    pthread_mutex_unlock(&snapshot_mutex);
    return;
  }

  printf("[P%d] Recebeu MARCADOR de P%d.\n", my_rank, origem);
  marcador_recebido[origem] = 1;

  if (!snapshot_ativo) {
    snapshot_ativo = 1;
    for (int i = 0; i < VECTOR_SIZE; i++)
      estado_local_snapshot[i] = vetor_local[i];
    imprimir_vetor(estado_local_snapshot, my_rank,
                   "SNAPSHOT: Estado local salvo");

    marcador_recebido[my_rank] = 1;

    pthread_mutex_lock(&mutexSaida);
    for (int i = 0; i < comm_size; i++) {
      while (countSaida == BUFFER_SIZE) {
        pthread_cond_wait(&condSaidaCheia, &mutexSaida);
      }
      filaSaida[inSaida].tipo = MSG_MARCADOR;
      filaSaida[inSaida].origem = i;
      for (int j = 0; j < VECTOR_SIZE; j++)
        filaSaida[inSaida].relogio[j] = relogio_marcador[j];
      inSaida = (inSaida + 1) % BUFFER_SIZE;
      countSaida++;
    }
    printf(
        "[P%d] SNAPSHOT: Participante P%d inundando a rede com marcadores.\n",
        my_rank, my_rank);
    pthread_cond_signal(&condSaidaVazia);
    pthread_mutex_unlock(&mutexSaida);
  }

  int snapshot_concluido = 1;
  for (int i = 0; i < comm_size; i++) {
    if (!marcador_recebido[i]) {
      snapshot_concluido = 0;
      break;
    }
  }

  if (snapshot_concluido) {
    imprimir_snapshot_final();
  }
  pthread_mutex_unlock(&snapshot_mutex);
}

void SNAPSHOT(int *vetor_local) {
  pthread_mutex_lock(&snapshot_mutex);

  snapshot_ativo = 1;
  printf("\n[P%d] INICIANDO ALGORITMO DE SNAPSHOT...\n", my_rank);
  for (int i = 0; i < VECTOR_SIZE; i++)
    estado_local_snapshot[i] = vetor_local[i];
  imprimir_vetor(estado_local_snapshot, my_rank,
                 "SNAPSHOT: Estado local salvo");

  marcador_recebido[my_rank] = 1;

  pthread_mutex_lock(&mutexSaida);
  for (int i = 0; i < comm_size; i++) {
    while (countSaida == BUFFER_SIZE) {
      pthread_cond_wait(&condSaidaCheia, &mutexSaida);
    }
    filaSaida[inSaida].tipo = MSG_MARCADOR;
    filaSaida[inSaida].origem = i;
    for (int j = 0; j < VECTOR_SIZE; j++)
      filaSaida[inSaida].relogio[j] = relogio_marcador[j];
    inSaida = (inSaida + 1) % BUFFER_SIZE;
    countSaida++;
  }
  printf("[P%d] SNAPSHOT: Iniciador enviando marcadores.\n", my_rank);
  pthread_cond_signal(&condSaidaVazia);
  pthread_mutex_unlock(&mutexSaida);

  pthread_mutex_unlock(&snapshot_mutex);
}

void *thread_entrada(void *arg) {
  while (1) {
    Mensagem msg;
    MPI_Status status;
    MPI_Recv(msg.relogio, VECTOR_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);

    msg.origem = status.MPI_SOURCE;
    msg.tipo = status.MPI_TAG;

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
    int tag = msg_para_enviar.tipo;

    MPI_Send(msg_para_enviar.relogio, VECTOR_SIZE, MPI_INT, destino, tag,
             MPI_COMM_WORLD);
  }
  return NULL;
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
  while (1) {
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

    if (msg_recebida.tipo == MSG_MARCADOR) {
      processar_marcador(vetor_local, msg_recebida.origem);
      continue;
    }

    pthread_mutex_lock(&snapshot_mutex);
    if (snapshot_ativo && !marcador_recebido[msg_recebida.origem]) {
      int chan = msg_recebida.origem;
      if (count_mensagens_canal[chan] < BUFFER_SIZE) {
        mensagens_canal[chan][count_mensagens_canal[chan]++] = msg_recebida;
        printf("[P%d] SNAPSHOT: Mensagem de P%d gravada no estado do canal "
               "%d->%d\n",
               my_rank, chan, chan, my_rank);
      }
    }
    pthread_mutex_unlock(&snapshot_mutex);

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
    break;
  }
}

void *thread_logica_programa(void *arg) {
  int vetor_local[VECTOR_SIZE] = {0};
  MPI_Barrier(MPI_COMM_WORLD);

  switch (my_rank) {
  case 0:
    evento_interno(vetor_local, "a");
    preparar_envio(vetor_local, 1, "b");
    SNAPSHOT(vetor_local);
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
    break;
  case 2:
    evento_interno(vetor_local, "k");
    preparar_envio(vetor_local, 0, "l");
    esperar_recebimento(vetor_local, "m");
    break;
  }

  MPI_Barrier(MPI_COMM_WORLD);

  sleep(2);

  MPI_Send(vetor_local, VECTOR_SIZE, MPI_INT, my_rank, MSG_TERMINO,
           MPI_COMM_WORLD);

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
    return EXIT_FAILURE;
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
    return EXIT_FAILURE;
  }
  pthread_t entrada, logica, saida;
  pthread_create(&entrada, NULL, thread_entrada, NULL);
  pthread_create(&logica, NULL, thread_logica_programa, NULL);
  pthread_create(&saida, NULL, thread_saida, NULL);
  pthread_join(entrada, NULL);
  pthread_join(logica, NULL);
  pthread_join(saida, NULL);
  MPI_Finalize();
  return EXIT_SUCCESS;
}

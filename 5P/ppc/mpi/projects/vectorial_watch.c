#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_SIZE 100

typedef struct message {
  char message_buffer[MSG_SIZE];
  int *vectorial_watch;
} VectorialMessage;

enum Actions {
  intn = 0,
  send = 1,
  recv = 2,
};

const int processess_config[13][3] = {
    {0, intn, false}, {2, intn, false}, {0, send, 1}, {1, send, 0},
    {0, recv, 1},     {1, recv, 0},     {0, send, 2}, {2, send, 0},
    {0, recv, 2},     {2, recv, 0},     {0, send, 1}, {1, recv, 0},
    {0, intn, false}};

int processess_config_size =
    sizeof(processess_config) / sizeof(processess_config[0]);

int create_array_string(int *array, int n, char *buffer, int buf_sz) {
  int buf_i = 0;

  // Verifica se há espaço mínimo para "{}\0"
  if (buf_sz < 3) {
    if (buf_sz > 0)
      buffer[0] = '\0';
    return -1; // erro: espaço insuficiente
  }

  buffer[buf_i++] = '{';

  for (int i = 0; i < n; i++) {
    // Adiciona separador ", " se não for o primeiro número
    if (i > 0) {
      if (buf_i + 2 >= buf_sz)
        goto truncate;
      buffer[buf_i++] = ',';
      buffer[buf_i++] = ' ';
    }

    // Escreve número no buffer
    int written = snprintf(&buffer[buf_i], buf_sz - buf_i, "%d", array[i]);
    if (written < 0 || buf_i + written >= buf_sz)
      goto truncate;
    buf_i += written;
  }

  if (buf_i + 2 >= buf_sz)
    goto truncate;
  buffer[buf_i++] = '}';
  buffer[buf_i] = '\0';
  return buf_i; // sucesso: retorna número de caracteres escritos

truncate:
  // Se buffer for pequeno, termina corretamente com '\0'
  if (buf_sz > 0)
    buffer[buf_sz - 1] = '\0';
  return -1; // erro: buffer truncado
}

int count_digits(int number) {
  if (number == 0)
    return 1;

  int digit_count = 0;

  if (number < 0) {
    digit_count++;
    number = abs(number);
  }

  digit_count += (int)floor(log10(number)) + 1;
  return digit_count;
}

int calculate_buffer_size(int *int_array, int array_length) {
  if (array_length == 0)
    return 3;

  int total_digit_chars = 0;

  for (int i = 0; i < array_length; i++) {
    total_digit_chars += count_digits(int_array[i]);
  }

  int comma_spaces = (array_length - 1) * 2;
  int surrounding_braces = 2;
  int null_terminator = 1;

  return total_digit_chars + comma_spaces + surrounding_braces +
         null_terminator;
}

int main() {
  int comm_size, my_rank;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int *vectorial_watch = calloc(comm_size, sizeof(int));

  for (int i = 0; i < processess_config_size; i++) {
    bool is_my_turn = processess_config[i][0] == my_rank;

    if (is_my_turn) {
      enum Actions action = processess_config[i][1];
      int address = processess_config[i][2];

      switch (action) {
      case intn: {
        // In internal action the process increments 1 in its position on
        // the vectorial watch:
        int buffer_size = calculate_buffer_size(vectorial_watch, comm_size);
        char *buffer = malloc(buffer_size * sizeof(char));

        printf("Code %d: Internal Process | %s\n", action, buffer);
        vectorial_watch[my_rank]++;
        break;
      }

      case send:
        MPI_Send(vectorial_watch, comm_size, MPI_INT, address, 0,
                 MPI_COMM_WORLD);

        int buffer_size = calculate_buffer_size(vectorial_watch, comm_size);
        char *buffer = malloc(buffer_size * sizeof(char) + 1);

        if (buffer == NULL) {
          perror("malloc failed");
          return 1;
        }

        create_array_string(vectorial_watch, comm_size, buffer, buffer_size);

        printf("Message sent from %d to %d | %s\n", my_rank, address, buffer);
        break;

      case recv: {
        int vectorial_watch_recieved[comm_size];

        int buffer_size =
            calculate_buffer_size(vectorial_watch_recieved, comm_size);
        char *buffer = malloc(buffer_size * sizeof(char) + 1);

        if (buffer == NULL) {
          perror("malloc failed");
          return 1;
        }

        create_array_string(vectorial_watch_recieved, comm_size, buffer,
                            buffer_size);

        MPI_Recv(vectorial_watch_recieved, 3, MPI_INT, address, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Message recieved by %d from %d | %s\n", my_rank, address,
               buffer);
        break;
      }
      }
    }
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}

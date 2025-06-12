#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  for (int i = 0; i < n; i++) {

    char temp[12];

    sprintf(temp, "%d", array[i]);
    strcat(buffer, temp);

    if (i < n - 1) {
      strcat(buffer, ".");
    }
  }

  buffer[buf_sz - 1] = '\0';

  return EXIT_SUCCESS;
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
    return 0;

  int total_digit_chars = 0;

  for (int i = 0; i < array_length; i++) {
    total_digit_chars += count_digits(int_array[i]);
  }

  int total_dots = array_length - 1;
  int terminator = 1;

  return total_digit_chars + total_dots + terminator;
}

int main() {
  int comm_size, my_rank;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int vectorial_watch[comm_size];

  for (int i = 0; i < comm_size; i++) {
    vectorial_watch[i] = 0;
  }

  for (int i = 0; i < processess_config_size; i++) {
    bool is_my_turn = processess_config[i][0] == my_rank;

    if (is_my_turn) {
      enum Actions action = processess_config[i][1];
      int address = processess_config[i][2];

      switch (action) {
      case intn: {
        // In internal action the process increments 1 in its position on
        // the vectorial watch:
        vectorial_watch[my_rank]++;

        int buffer_size = calculate_buffer_size(vectorial_watch, comm_size);
        char *buffer = calloc(buffer_size, sizeof(char));

        create_array_string(vectorial_watch, comm_size, buffer, buffer_size);

        printf("Code %d: Internal Process | %s\n", action, buffer);
        break;
      }

      case send:
        MPI_Send(vectorial_watch, comm_size, MPI_INT, address, 0,
                 MPI_COMM_WORLD);

        int buffer_size = calculate_buffer_size(vectorial_watch, comm_size);
        char *buffer = calloc(buffer_size, sizeof(char));

        if (buffer == NULL) {
          perror("malloc failed");
          return 1;
        }

        create_array_string(vectorial_watch, comm_size, buffer, buffer_size);

        printf("Code %d | Message sent from %d to %d | %s\n", action, my_rank,
               address, buffer);
        break;

      case recv: {
        vectorial_watch[my_rank]++;

        int vectorial_watch_recieved[comm_size];

        MPI_Recv(vectorial_watch_recieved, 3, MPI_INT, address, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int buffer_size = calculate_buffer_size(vectorial_watch, comm_size);
        char *buffer = calloc(buffer_size, sizeof(char));

        if (buffer == NULL) {
          perror("malloc failed");
          return EXIT_FAILURE;
        }

        create_array_string(vectorial_watch, comm_size, buffer, buffer_size);

        printf("Code %d | Message recieved by %d from %d | %s\n", action,
               my_rank, address, buffer);
        break;
      }
      }
    }
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}

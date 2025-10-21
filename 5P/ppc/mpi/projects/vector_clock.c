/**
 * Código base (incompleto) para implementação de relógios vetoriais.
 * Meta: implementar a interação entre três processos ilustrada na figura
 * da URL:
 *
 * https://people.cs.rutgers.edu/~pxk/417/notes/images/clocks-vector.png
 *
 * Compilação: mpicc -o rvet rvet.c
 * Execução:   mpiexec -n 3 ./rvet
 */

#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_digits(int number) {
  if (number == 0) {
    return 1;
  }
  int digit_count = (number < 0) ? 1 : 0;
  number = abs(number);
  digit_count += (int)floor(log10(number)) + 1;
  return digit_count;
}

int calculate_buffer_size(int *int_array, int array_length) {
  int total_digit_chars = 0;
  for (int i = 0; i < array_length; i++)
    total_digit_chars += count_digits(int_array[i]);
  return total_digit_chars + (array_length - 1) + 1;
}

char *create_array_string(int *array, int n) {
  if (!array || n <= 0) {
    return NULL;
  }
  int buf_sz = calculate_buffer_size(array, n);
  char *buffer = (char *)malloc(buf_sz);
  if (!buffer) {
    return NULL;
  }
  buffer[0] = '\0';
  for (int i = 0; i < n; i++) {
    char temp[12];
    sprintf(temp, "%d", array[i]);
    strcat(buffer, temp);
    if (i < n - 1) {
      strcat(buffer, ".");
    }
  }
  return buffer;
}

void internal_event(int *vector_clock, int comm_size, int my_rank) {
  vector_clock[my_rank]++;
  char *buffer = create_array_string(vector_clock, comm_size);
  printf("Internal process from process %d | %s\n", my_rank, buffer);
  free(buffer);
}

void send_event(int *vector_clock, int comm_size, int my_rank,
                int target_rank) {
  vector_clock[my_rank]++;
  char *buffer = create_array_string(vector_clock, comm_size);
  MPI_Send(vector_clock, comm_size + 1, MPI_INT, target_rank, 0,
           MPI_COMM_WORLD);
  printf("Process %d sending message to process %d | %s\n", my_rank,
         target_rank, buffer);
  free(buffer);
}

void receive_event(int *vector_clock, int *recv_buffer, int comm_size,
                   int my_rank, int source_rank) {
  MPI_Recv(recv_buffer, comm_size + 1, MPI_INT, source_rank, 0, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
  for (int i = 0; i < comm_size; i++)
    if (recv_buffer[i] > vector_clock[i])
      vector_clock[i] = recv_buffer[i];
  vector_clock[my_rank]++;
  char *buffer = create_array_string(vector_clock, comm_size);
  printf("Process %d receiving message from process %d | %s\n", my_rank,
         source_rank, buffer);
  free(buffer);
}

int main() {
  int comm_size, my_rank;
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int *vector_clock = (int *)calloc(comm_size, sizeof(int));
  int *new_vector_clock = (int *)calloc(comm_size, sizeof(int));
  vector_clock[comm_size] = my_rank;

  switch (my_rank) {
  case 0:
    internal_event(vector_clock, comm_size, my_rank);
    send_event(vector_clock, comm_size, my_rank, 1);
    receive_event(vector_clock, new_vector_clock, comm_size, my_rank, 1);
    send_event(vector_clock, comm_size, my_rank, 2);
    receive_event(vector_clock, new_vector_clock, comm_size, my_rank, 2);
    send_event(vector_clock, comm_size, my_rank, 1);
    internal_event(vector_clock, comm_size, my_rank);
    break;

  case 1:
    send_event(vector_clock, comm_size, my_rank, 0);
    receive_event(vector_clock, new_vector_clock, comm_size, my_rank, 0);
    receive_event(vector_clock, new_vector_clock, comm_size, my_rank, 0);
    break;

  case 2:
    internal_event(vector_clock, comm_size, my_rank);
    send_event(vector_clock, comm_size, my_rank, 0);
    receive_event(vector_clock, new_vector_clock, comm_size, my_rank, 0);
    break;
  }

  free(vector_clock);
  free(new_vector_clock);
  MPI_Finalize();
  return EXIT_SUCCESS;
}

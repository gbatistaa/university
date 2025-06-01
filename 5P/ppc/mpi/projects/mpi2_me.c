#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <string.h>

const int MAX_STRING = 100;

int main(void) {
  int comm_size, my_rank, num_buf, x = 5, total = 0;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  switch (my_rank) {
  case 0:
    /* Impressão da mensagem do processo centralizador */
    printf("Processo centralizador em execução: %d de %d!\n", my_rank,
           comm_size);

    for (int q = 1; q < comm_size; q++) {
      /* Recepção da mensagem do processo q */
      MPI_Recv(&num_buf, MAX_STRING, MPI_INT, q, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);

      total += num_buf;
    }

    printf("Total da expressão 2x + x³ + x^x, com x igual a %d, foi: %d\n", x,
           total);
    break;

  case 1:
    num_buf = 2 * x;
    MPI_Send(&num_buf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    /* Impressão da mensagem do processo q */
    printf("O processo 1 somou o seguinte valor: %d\n", num_buf);
    break;

  case 2:
    num_buf = pow(x, 3);
    MPI_Send(&num_buf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    /* Impressão da mensagem do processo q */
    printf("O processo 2 somou o seguinte valor: %d\n", num_buf);
    break;

  case 3:
    num_buf = pow(x, x);
    MPI_Send(&num_buf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    /* Impressão da mensagem do processo q */
    printf("O processo 3 somou o seguinte valor: %d\n", num_buf);
    break;
  }

  /* Finaliza MPI */
  MPI_Finalize();

  return 0;
}

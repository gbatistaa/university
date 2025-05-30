#include <mpi.h>
#include <stdio.h>
#include <string.h>
#define MAX_STRING 100

double math_expression(int num) {
  double result = (double)(num * num) - (4 * num) + 8;
  return result;
}

double calculate_area(double lado_1, double lado_2) {
  double area = (double)lado_1 * lado_2;
  return area;
}

int main() {
  char message[MAX_STRING];
  int comm_size;
  int my_rank;
  double curr_area;
  double integral_area = 0;
  double precision = 1000;
  double sum_areas = 0;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int a = 1, b = comm_size;

  if (my_rank != 0) {
    /* Criação da mensagem */
    int range = precision / b;
    int curr_sum = 0;
    for (int i = range * (my_rank - 1); i < range * my_rank; i++) {
      double curr_exp_value = math_expression(i / precision);
      curr_area = calculate_area(curr_exp_value, 1 / precision);
      curr_sum += curr_area;
    }
    sum_areas = curr_sum;
    /* Envio da mensagem para o processo 0 */
    MPI_Send(&sum_areas, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
    /* Impressão da mensagem do processo centralizador */
    printf("Processo centralizador em execução: %d of %d!\n", my_rank,
           comm_size);

    MPI_Status status;

    for (int q = 1; q < comm_size; q++) {
      /* Recepção da mensagem do processo q */

      MPI_Recv(&sum_areas, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
               MPI_COMM_WORLD, &status);

      integral_area += sum_areas;

      /* Impressão da mensagem do processo q */
      printf("O processo %d recebeu o seguinte valor de área integral: %f "
             "(status.MPI_SOURCE: %d status.MPI_TAG: %d)\n",
             my_rank, sum_areas, status.MPI_SOURCE, status.MPI_TAG);
    }

    printf("A integral é: %f\n", integral_area);
  }

  return 0;
}

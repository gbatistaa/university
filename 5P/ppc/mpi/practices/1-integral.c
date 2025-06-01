#include <mpi.h>
#include <stdio.h>
#include <string.h>

float math_expression(float num) {
  float result = (float)(num * num) - (4 * num) + 8;
  return result;
}

float calculate_area(float lado_1, float lado_2) {
  float area = (float)lado_1 * lado_2;
  return area;
}

int main() {
  int comm_size;
  int my_rank;
  float curr_area;

  int precision = 1000;
  float sum_areas;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int a = 1, b = comm_size;

  if (my_rank != 0) {
    /* Criação da mensagem */
    int range = precision / (b - 1);
    float curr_sum = 0;

    printf("%d | %d | %d\n", range * (my_rank - 1), range * my_rank, my_rank);

    for (int i = range * (my_rank - 1); i < range * my_rank; i++) {
      float curr_exp_value = math_expression((float)i / range);
      curr_area = calculate_area(curr_exp_value, (float)1 / range);
      // printf("%f * %f = %f\n", curr_exp_value, curr_exp_value / range,
      //        curr_area);

      curr_sum += curr_area;
    }

    sum_areas = curr_sum;

    /* Envio da mensagem para o processo 0 */
    MPI_Send(&sum_areas, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
  } else {
    /* Impressão da mensagem do processo centralizador */
    printf("Processo centralizador em execução: %d of %d!\n", my_rank,
           comm_size);

    MPI_Status status;
    float integral_area = 0;

    for (int q = 1; q < comm_size; q++) {
      /* Recepção da mensagem do processo q */

      MPI_Recv(&sum_areas, 1, MPI_FLOAT, q, MPI_ANY_TAG, MPI_COMM_WORLD,
               &status);

      integral_area += sum_areas;

      /* Impressão da mensagem do processo q */
      // printf("O processo %d recebeu o seguinte valor de área integral: %f "
      //        "(status.MPI_SOURCE: %d status.MPI_TAG: %d)\n",
      //        q, sum_areas, status.MPI_SOURCE, status.MPI_TAG);
    }

    printf("A integral é: %f\n", integral_area);
  }

  MPI_Finalize();

  return 0;
}

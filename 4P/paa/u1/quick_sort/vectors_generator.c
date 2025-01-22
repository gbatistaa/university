#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int isDuplicated(int size, int *vector, int num) {
  for (int i = 0; i < size; i++) {
    if (vector[i] == num) {
      return 1; // Número duplicado
    }
  }
  return 0; // Não duplicado
}

void generateRandomVector(int n, int *vector) {
  int randomNum;
  for (int i = 0; i < n; i++) {
    do {
      randomNum = rand() % 201 - 50; // Gera números de -50 a 150
    } while (isDuplicated(i, vector, randomNum));
    vector[i] = randomNum;
  }
}

void generateQuickSortFile(FILE *file, int vectorsQuant) {
  fprintf(file, "%d\n", vectorsQuant);

  for (int i = 0; i < vectorsQuant; i++) {
    int randomVectorSize = rand() % 1997 + 3; // Tamanho do vetor entre 3 e 2000
    int vector[randomVectorSize];
    generateRandomVector(randomVectorSize, vector);

    fprintf(file, "%d\n", randomVectorSize); // Escreve o tamanho do vetor
    for (int j = 0; j < randomVectorSize - 1; j++) {
      fprintf(file, "%d ", vector[j]); // Escreve cada elemento do vetor
    }
    fprintf(file, "%d\n", vector[randomVectorSize - 1]); // Último elemento
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Erro: Não foi fornecido o nome do arquivo de saída.\n");
    return 1;
  }

  FILE *file = fopen(argv[1], "w");
  if (!file) {
    perror("Erro ao abrir arquivo");
    return 1;
  }

  srand(time(NULL)); // Inicializa o gerador de números aleatórios

  int vectorsQuant;
  printf("Quantidade de vetores: ");
  if (scanf("%d", &vectorsQuant) != 1) {
    printf("Valor inserido não é numérico.\n");
    fclose(file);
    return 1;
  }

  generateQuickSortFile(file, vectorsQuant);

  fclose(file);
  return 0;
}

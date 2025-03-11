#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STORED -1

// Definição das estruturas (equivalentes às classes em C++)
typedef struct {
  char sign[64];
  int max_weight;
  int max_volume;
} Vehicle;

typedef struct {
  Vehicle *list;
  int size;
} VehicleList;

typedef struct {
  char code[64];
  float value;
  int weight;
  int volume;
} Package;

typedef struct {
  Package *list;
  int size;
} PackageList;

// Função para ler o arquivo de entrada e preencher as estruturas
int read_file(FILE *input, VehicleList *vehicle_list,
              PackageList *package_list) {
  char line[256];
  // Lê a primeira linha com o número de veículos
  if (fgets(line, sizeof(line), input) == NULL)
    return EXIT_FAILURE;
  sscanf(line, "%d", &vehicle_list->size);
  vehicle_list->list = (Vehicle *)malloc(vehicle_list->size * sizeof(Vehicle));

  int v = 0;
  // Lê as linhas dos veículos até encontrar uma linha com menos de 5 caracteres
  while (1) {
    if (fgets(line, sizeof(line), input) == NULL)
      break;
    if (strlen(line) < 5)
      break;
    sscanf(line, "%s %d %d", vehicle_list->list[v].sign,
           &vehicle_list->list[v].max_weight,
           &vehicle_list->list[v].max_volume);
    v++;
  }
  // A linha lida (curta) contém o tamanho da lista de pacotes
  package_list->size = atoi(line);
  package_list->list = (Package *)malloc(package_list->size * sizeof(Package));

  int p = 0;
  // Lê todas as linhas dos pacotes
  while (fgets(line, sizeof(line), input) != NULL) {
    sscanf(line, "%s %f %d %d", package_list->list[p].code,
           &package_list->list[p].value, &package_list->list[p].weight,
           &package_list->list[p].volume);
    p++;
  }
  return EXIT_SUCCESS;
}

// Função que realiza a maximização do valor para um veículo, preservando a
// lógica original
int maximize_vehicle_value(PackageList *package_list, Vehicle vehicle,
                           char *output_string) {
  int n = package_list->size;
  int W = vehicle.max_weight + 1; // capacidades de 0 a max_weight
  int V = vehicle.max_volume + 1; // capacidades de 0 a max_volume

  // Aloca duas matrizes 2D para DP: dp_prev (camada anterior) e dp_curr (camada
  // atual)
  float **dp_prev = (float **)malloc(W * sizeof(float *));
  float **dp_curr = (float **)malloc(W * sizeof(float *));
  for (int w = 0; w < W; w++) {
    dp_prev[w] = (float *)malloc(V * sizeof(float));
    dp_curr[w] = (float *)malloc(V * sizeof(float));
    for (int v = 0; v < V; v++) {
      dp_prev[w][v] = 0.0f;
      dp_curr[w][v] = 0.0f;
    }
  }

  // Aloca uma matriz de decisões 3D: decision[i][w][v] guarda o índice do
  // pacote escolhido
  int ***decision = (int ***)malloc((n + 1) * sizeof(int **));
  for (int i = 0; i <= n; i++) {
    decision[i] = (int **)malloc(W * sizeof(int *));
    for (int w = 0; w < W; w++) {
      decision[i][w] = (int *)malloc(V * sizeof(int));
      for (int v = 0; v < V; v++) {
        decision[i][w][v] = -1;
      }
    }
  }

  // Processa os pacotes um a um (i de 1 a n)
  for (int i = 1; i <= n; i++) {
    Package pkg = package_list->list[i - 1];
    for (int w = 0; w < W; w++) {
      for (int v = 0; v < V; v++) {
        // Se não pegar o pacote, mantém o valor anterior
        dp_curr[w][v] = dp_prev[w][v];
        decision[i][w][v] = -1;
        // Se o pacote cabe nesta capacidade e ainda não foi armazenado
        if (pkg.value != STORED && w >= pkg.weight && v >= pkg.volume) {
          float candidate = dp_prev[w - pkg.weight][v - pkg.volume] + pkg.value;
          if (candidate > dp_curr[w][v]) {
            dp_curr[w][v] = candidate;
            decision[i][w][v] =
                i - 1; // registra que o pacote (i-1) foi escolhido
          }
        }
      }
    }
    // Copia a camada atual para a anterior para o próximo item
    for (int w = 0; w < W; w++) {
      for (int v = 0; v < V; v++) {
        dp_prev[w][v] = dp_curr[w][v];
      }
    }
  }

  float max_value = dp_prev[W - 1][V - 1];
  char temp[128];
  sprintf(temp, "[%s]R$%.2f,", vehicle.sign, max_value);
  strcat(output_string, temp);

  // Backtracking: percorre os pacotes em ordem inversa (de i = n até 1)
  int w = W - 1, v = V - 1;
  int i = n;
  float used_weight = 0, used_volume = 0;
  // Para armazenar os códigos dos pacotes escolhidos
  const char *chosen[n];
  int chosen_count = 0;
  while (i > 0 && w >= 0 && v >= 0) {
    if (decision[i][w][v] != -1) {
      int pkgIndex = decision[i][w][v];
      Package pkg = package_list->list[pkgIndex];
      chosen[chosen_count++] = pkg.code;
      used_weight += pkg.weight;
      used_volume += pkg.volume;
      // Marca o pacote como armazenado para que não seja considerado nas
      // pendências
      package_list->list[pkgIndex].value = STORED;
      w -= pkg.weight;
      v -= pkg.volume;
    }
    i--;
  }
  // Concatena os códigos escolhidos na ordem (invertida em relação à ordem de
  // backtracking)
  char used_codes[10000] = "";
  for (int j = chosen_count - 1; j >= 0; j--) {
    strcat(used_codes, chosen[j]);
    strcat(used_codes, ",");
  }
  if (strlen(used_codes) > 0)
    used_codes[strlen(used_codes) - 1] = '\0'; // remove a última vírgula

  int uw_perct = (int)ceil((used_weight / vehicle.max_weight) * 100.0);
  int uv_perct = (int)ceil((used_volume / vehicle.max_volume) * 100.0);
  sprintf(temp, "%dKG", (int)used_weight);
  strcat(output_string, temp);
  sprintf(temp, "(%d%%),", uw_perct);
  strcat(output_string, temp);
  sprintf(temp, "%dL", (int)used_volume);
  strcat(output_string, temp);
  sprintf(temp, "(%d%%)->", uv_perct);
  strcat(output_string, temp);
  strcat(output_string, used_codes);

  // Libera a memória alocada para as matrizes de DP
  for (int w = 0; w < W; w++) {
    free(dp_prev[w]);
    free(dp_curr[w]);
  }
  free(dp_prev);
  free(dp_curr);
  // Libera a memória da matriz de decisão
  for (int i = 0; i <= n; i++) {
    for (int w = 0; w < W; w++) {
      free(decision[i][w]);
    }
    free(decision[i]);
  }
  free(decision);
  return EXIT_SUCCESS;
}

// Função que calcula as pendências, acumulando o valor, peso, volume e os
// códigos dos pacotes não armazenados
int calculate_pendencies(PackageList *package_list, char *output_string) {
  float pendent_value = 0, pendent_weight = 0, pendent_volume = 0;
  const char *pendents[package_list->size];
  int pendent_count = 0;
  for (int i = package_list->size - 1; i >= 0; i--) {
    if (package_list->list[i].value != STORED) {
      pendent_value += package_list->list[i].value;
      pendent_weight += package_list->list[i].weight;
      pendent_volume += package_list->list[i].volume;
      pendents[pendent_count++] = package_list->list[i].code;
    }
  }
  char pendents_codes[10000] = "";
  for (int j = pendent_count - 1; j >= 0; j--) {
    strcat(pendents_codes, pendents[j]);
    strcat(pendents_codes, ",");
  }
  if (strlen(pendents_codes) > 0)
    pendents_codes[strlen(pendents_codes) - 1] = '\0';

  char temp[128];
  sprintf(temp, "[PENDENTE]:R$%.2f,", pendent_value);
  strcat(output_string, temp);
  sprintf(temp, "%dKG,", (int)pendent_weight);
  strcat(output_string, temp);
  sprintf(temp, "%dL->", (int)pendent_volume);
  strcat(output_string, temp);
  strcat(output_string, pendents_codes);
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  clock_t start = clock();
  if (argc < 3) {
    fprintf(stderr, "Uso: %s <arquivo_input> <arquivo_output>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *input = fopen(argv[1], "r");
  if (!input) {
    fprintf(stderr, "erro ao abrir input\n");
    return EXIT_FAILURE;
  }
  FILE *output = fopen(argv[2], "w");
  if (!output) {
    fprintf(stderr, "erro ao abrir output\n");
    fclose(input);
    return EXIT_FAILURE;
  }
  printf("Input aberto com sucesso!\n");
  printf("Output aberto com sucesso!\n");

  VehicleList *vehicle_list = (VehicleList *)malloc(sizeof(VehicleList));
  PackageList *package_list = (PackageList *)malloc(sizeof(PackageList));

  // Buffer para acumular a saída (certifique-se de que o tamanho seja
  // suficiente)
  char output_string[1000000];
  output_string[0] = '\0';

  read_file(input, vehicle_list, package_list);

  // Para cada veículo, chama a função de maximização
  for (int i = 0; i < vehicle_list->size; i++) {
    maximize_vehicle_value(package_list, vehicle_list->list[i], output_string);
    strcat(output_string, "\n");
  }
  calculate_pendencies(package_list, output_string);

  fputs(output_string, output);

  fclose(input);
  fclose(output);

  clock_t end = clock();
  double duration = (double)(end - start) / CLOCKS_PER_SEC;
  printf("Execution time: %f s\n", duration);

  // Libera a memória alocada
  free(vehicle_list->list);
  free(vehicle_list);
  free(package_list->list);
  free(package_list);

  return EXIT_SUCCESS;
}

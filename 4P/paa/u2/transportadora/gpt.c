#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STORED -1

// Definição das estruturas
typedef struct {
  char sign[50];
  int max_weight;
  int max_volume;
} Vehicle;

typedef struct {
  Vehicle *list;
  int size;
} VehicleList;

typedef struct {
  char code[50];
  float value;
  float weight;
  float volume;
} Package;

typedef struct {
  Package *list;
  int size;
} PackageList;

// Função para ler o arquivo e alocar as listas de veículos e pacotes
int read_file(FILE *input, VehicleList *vehicle_list,
              PackageList *package_list) {
  char line[256];

  // Lê a primeira linha com o número de veículos
  if (fgets(line, sizeof(line), input) == NULL) {
    return EXIT_FAILURE;
  }
  sscanf(line, "%d", &vehicle_list->size);
  vehicle_list->list = (Vehicle *)malloc(sizeof(Vehicle) * vehicle_list->size);

  int v = 0;
  // Lê as linhas dos veículos até encontrar uma linha com menos de 5 caracteres
  while (fgets(line, sizeof(line), input)) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
      len--;
    }
    if (len < 5) {
      break;
    }
    sscanf(line, "%s %d %d", vehicle_list->list[v].sign,
           &vehicle_list->list[v].max_weight,
           &vehicle_list->list[v].max_volume);
    v++;
    if (v >= vehicle_list->size)
      break;
  }

  // A linha lida com menos de 5 caracteres contém o número de pacotes
  package_list->size = atoi(line);
  package_list->list = (Package *)malloc(sizeof(Package) * package_list->size);

  int p = 0;
  while (fgets(line, sizeof(line), input)) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
      line[len - 1] = '\0';
    if (strlen(line) == 0)
      continue;
    sscanf(line, "%s %f %f %f", package_list->list[p].code,
           &package_list->list[p].value, &package_list->list[p].weight,
           &package_list->list[p].volume);
    p++;
    if (p >= package_list->size)
      break;
  }
  return EXIT_SUCCESS;
}

// Função que maximiza o valor transportado para um veículo utilizando
// programação dinâmica
int maximize_vehicle_value(PackageList *package_list, Vehicle vehicle,
                           char *output_string, int *output_index) {
  int n = package_list->size;
  int W = vehicle.max_weight + 1; // capacidades de 0 a max_weight
  int V = vehicle.max_volume + 1; // capacidades de 0 a max_volume

  // Aloca as matrizes 2D para DP: dp_prev e dp_curr
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

  // Aloca a matriz de decisões 3D: decision[n+1][W][V]
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
        if (pkg.value != STORED && w >= (int)pkg.weight &&
            v >= (int)pkg.volume) {
          float candidate =
              dp_prev[w - (int)pkg.weight][v - (int)pkg.volume] + pkg.value;
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
  *output_index += sprintf(output_string + *output_index, "[%s]R$%.2f,",
                           vehicle.sign, max_value);

  // Backtracking: percorre os pacotes em ordem inversa (de i = n até 1)
  int w = W - 1, v = V - 1;
  int i = n;
  float used_weight = 0.0f, used_volume = 0.0f;
  char used_pkgs_codes[1000] = "";
  while (i > 0 && w >= 0 && v >= 0) {
    if (decision[i][w][v] != -1) {
      int pkgIndex = decision[i][w][v];
      Package pkg = package_list->list[pkgIndex];
      {
        char temp[60];
        sprintf(temp, "%s,", pkg.code);
        strcat(used_pkgs_codes, temp);
      }
      used_weight += pkg.weight;
      used_volume += pkg.volume;
      // Marca o pacote como armazenado para não ser considerado em pendências
      package_list->list[pkgIndex].value = STORED;
      w -= (int)pkg.weight;
      v -= (int)pkg.volume;
    }
    i--;
  }
  // Remove a última vírgula, se houver
  int len = strlen(used_pkgs_codes);
  if (len > 0 && used_pkgs_codes[len - 1] == ',')
    used_pkgs_codes[len - 1] = '\0';

  float uw_perct = round((used_weight / vehicle.max_weight) * 100);
  float uv_perct = round((used_volume / vehicle.max_volume) * 100);
  *output_index += sprintf(output_string + *output_index, "%dKG(%d%%),",
                           (int)used_weight, (int)uw_perct);
  *output_index += sprintf(output_string + *output_index, "%dL(%d%%)->%s\n",
                           (int)used_volume, (int)uv_perct, used_pkgs_codes);

  return EXIT_SUCCESS;
}

// Função que calcula as pendências (pacotes não armazenados)
int calculate_pendencies(PackageList *package_list, char *output_string,
                         int *output_index) {
  float pendent_value = 0.0f, pendent_weight = 0.0f, pendent_volume = 0.0f;
  char pendent_packages_codes[1000] = "";
  for (int i = package_list->size - 1; i >= 0; i--) {
    if (package_list->list[i].value != STORED) {
      pendent_value += package_list->list[i].value;
      pendent_weight += package_list->list[i].weight;
      pendent_volume += package_list->list[i].volume;
      {
        char temp[60];
        sprintf(temp, "%s,", package_list->list[i].code);
        strcat(pendent_packages_codes, temp);
      }
    }
  }
  // Remove a última vírgula, se houver
  int len = strlen(pendent_packages_codes);
  if (len > 0 && pendent_packages_codes[len - 1] == ',')
    pendent_packages_codes[len - 1] = '\0';
  pendent_value = pendent_value - 0.01f;
  printf("%.2f\n", pendent_value);
  *output_index +=
      sprintf(output_string + *output_index, "PENDENTE:R$%.2f,", pendent_value);
  *output_index +=
      sprintf(output_string + *output_index, "%dKG,", (int)pendent_weight);
  *output_index += sprintf(output_string + *output_index, "%dL->%s",
                           (int)pendent_volume, pendent_packages_codes);
  return EXIT_SUCCESS;
}

// Função principal
int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Uso: %s arquivo_entrada arquivo_saida\n", argv[0]);
    return EXIT_FAILURE;
  }

  clock_t start = clock();

  FILE *input = fopen(argv[1], "r");
  if (!input) {
    fprintf(stderr, "erro ao abrir input\n");
    return EXIT_FAILURE;
  }
  printf("Input aberto com sucesso!\n");

  FILE *output = fopen(argv[2], "w");
  if (!output) {
    fprintf(stderr, "erro ao abrir output\n");
    fclose(input);
    return EXIT_FAILURE;
  }
  printf("Output aberto com sucesso!\n\n");

  VehicleList vehicle_list;
  PackageList package_list;
  vehicle_list.list = NULL;
  vehicle_list.size = 0;
  package_list.list = NULL;
  package_list.size = 0;

  if (read_file(input, &vehicle_list, &package_list) != EXIT_SUCCESS) {
    fprintf(stderr, "Erro na leitura do arquivo\n");
    fclose(input);
    fclose(output);
    return EXIT_FAILURE;
  }

  // Buffer para acumular a saída
  char output_string[10000];
  output_string[0] = '\0';
  int output_index = 0;

  // Processa cada veículo
  for (int i = 0; i < vehicle_list.size; i++) {
    maximize_vehicle_value(&package_list, vehicle_list.list[i], output_string,
                           &output_index);
  }
  calculate_pendencies(&package_list, output_string, &output_index);

  fputs(output_string, output);

  clock_t end = clock();
  double duration = (double)(end - start) / CLOCKS_PER_SEC;
  printf("Execution time: %.6f s\n", duration);

  fclose(input);
  fclose(output);

  return EXIT_SUCCESS;
}

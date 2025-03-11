#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STORED -1

typedef struct {
  char sign[20];
  int max_weight;
  int max_volume;
} Vehicle;

typedef struct {
  Vehicle *list;
  int size;
} VehicleList;

typedef struct {
  char code[20];
  float value;
  float weight;
  float volume;
} Package;

typedef struct {
  Package *list;
  int size;
} PackageList;

void read_file(FILE *input, VehicleList *vehicle_list,
               PackageList *package_list) {
  char line[100];
  fgets(line, sizeof(line), input);
  sscanf(line, "%d", &vehicle_list->size);

  vehicle_list->list = (Vehicle *)malloc(vehicle_list->size * sizeof(Vehicle));
  for (int v = 0; v < vehicle_list->size; v++) {
    fgets(line, sizeof(line), input);
    sscanf(line, "%s %d %d", vehicle_list->list[v].sign,
           &vehicle_list->list[v].max_weight,
           &vehicle_list->list[v].max_volume);
  }

  fgets(line, sizeof(line), input);
  sscanf(line, "%d", &package_list->size);

  package_list->list = (Package *)malloc(package_list->size * sizeof(Package));
  for (int p = 0; p < package_list->size; p++) {
    fgets(line, sizeof(line), input);
    sscanf(line, "%s %f %f %f", package_list->list[p].code,
           &package_list->list[p].value, &package_list->list[p].weight,
           &package_list->list[p].volume);
  }
}

void maximize_vehicle_value(PackageList *package_list, Vehicle vehicle,
                            char *output_string) {
  int n = package_list->size;
  int W = vehicle.max_weight + 1;
  int V = vehicle.max_volume + 1;

  float **dp = (float **)malloc(W * sizeof(float *));
  int **chosen = (int **)malloc(W * sizeof(int *));
  for (int w = 0; w < W; w++) {
    dp[w] = (float *)calloc(V, sizeof(float));
    chosen[w] = (int *)calloc(V, sizeof(int));
  }

  for (int i = 0; i < n; i++) {
    Package pkg = package_list->list[i];
    for (int w = W - 1; w >= (int)pkg.weight; w--) {
      for (int v = V - 1; v >= (int)pkg.volume; v--) {
        float new_value =
            dp[w - (int)pkg.weight][v - (int)pkg.volume] + pkg.value;
        if (new_value > dp[w][v]) {
          dp[w][v] = new_value;
          chosen[w][v] = i + 1;
        }
      }
    }
  }

  float max_value = dp[W - 1][V - 1];
  sprintf(output_string + strlen(output_string), "[%s]R$%.2f,", vehicle.sign,
          max_value);

  int w = W - 1, v = V - 1;
  char used_pkgs_codes[1024] = "";
  while (w > 0 && v > 0 && chosen[w][v] > 0) {
    int i = chosen[w][v] - 1;
    strcat(used_pkgs_codes, package_list->list[i].code);
    strcat(used_pkgs_codes, ",");
    w -= (int)package_list->list[i].weight;
    v -= (int)package_list->list[i].volume;
  }

  if (strlen(used_pkgs_codes) > 0)
    used_pkgs_codes[strlen(used_pkgs_codes) - 1] = '\0';

  sprintf(output_string + strlen(output_string), "->%s\n", used_pkgs_codes);

  for (int w = 0; w < W; w++) {
    free(dp[w]);
    free(chosen[w]);
  }
  free(dp);
  free(chosen);
}

int main(int argc, char *argv[]) {
  clock_t start = clock();
  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");
  if (!input || !output) {
    fprintf(stderr, "Erro ao abrir arquivos\n");
    return EXIT_FAILURE;
  }

  PackageList package_list;
  VehicleList vehicle_list;
  char output_string[4096] = "";
  read_file(input, &vehicle_list, &package_list);

  for (int i = 0; i < vehicle_list.size; i++) {
    maximize_vehicle_value(&package_list, vehicle_list.list[i], output_string);
  }

  fputs(output_string, output);
  fclose(input);
  fclose(output);

  printf("Execution time: %.2f s\n",
         (double)(clock() - start) / CLOCKS_PER_SEC);
  return EXIT_SUCCESS;
}

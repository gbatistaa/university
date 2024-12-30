#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE *input;

typedef struct container {
  char *code;
  char *cnpj;
  int weight;
} Container;

void merge(Container array[], int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  Container leftSubArray[n1], rightSubArray[n2];

  for (int i = 0; i < n1; i++)
    leftSubArray[i] = array[left + i];
  for (int j = 0; j < n2; j++)
    rightSubArray[j] = array[mid + 1 + j];

  int i = 0;
  int j = 0;
  int k = left;

  while (i < n1 && j < n2) {
    if (leftSubArray[i].weight <= rightSubArray[j].weight) {
      array[k] = leftSubArray[i];
      i++;
    } else {
      array[k] = rightSubArray[j];
      j++;
    }
    k++;
  }
  while (i < n1) {
    array[k] = leftSubArray[i];
    i++;
    k++;
  }

  while (j < n2) {
    array[k] = rightSubArray[j];
    j++;
    k++;
  }
}

void mergeSort(Container arr[], int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    merge(arr, left, mid, right);
  }
}

// Function to manage the input file
void readInputAndCreateContainerList(Container **containerList) {
  input = fopen("porto.txt", "r");

  int i = 0;
  char fileLineContent[50];
  while (fgets(fileLineContent, 50, input)) {

    if (strlen(fileLineContent) <= 3) {
      continue; // Ignora linhas muito curtas
    }

    // printf("%s | %lu\n", auxiliar, strlen(auxiliar));

    // 0 indicates code, 1 cnpj and 2 indicates weight
    int propStatus = 0, j = 0;
    char *currentCode = (char *)malloc(sizeof(char));
    char *currentCnpj = (char *)malloc(sizeof(char));
    char *currentWeightString = (char *)malloc(sizeof(char));
    int currentWeight;

    size_t currentSize = sizeof(char);

    for (int k = 0; k < sizeof(fileLineContent); k++) {
      switch (propStatus) {

      // Saving the container code
      case 0:
        if (fileLineContent[k] == ' ') {
          propStatus++;
          currentCnpj[j + 1] = '\0';
          j = 0;
          currentSize = 0;
        } else {
          currentCode[j] = fileLineContent[k];
          currentCode =
              (char *)realloc(currentCode, currentSize + sizeof(char));
          currentSize += sizeof(char);
          j++;
        }
        break;

      // Saving the container cnpj
      case 1:
        if (fileLineContent[k] == ' ') {
          propStatus++;
          currentCode[j + 1] = '\0';
          j = 0;
          currentSize = 0;
        } else {
          currentCnpj[j] = fileLineContent[k];
          currentCnpj =
              (char *)realloc(currentCnpj, currentSize + sizeof(char));
          currentSize += sizeof(char);
          j++;
        }
        break;

      // Saving the container weight
      case 2:
        if (fileLineContent[k] == ' ') {
          propStatus++;
          currentWeightString[j + 1] = '\0';
          j = 0;
          currentSize = 0;
        } else {
          currentWeightString[j] = fileLineContent[k];
          currentWeightString =
              (char *)realloc(currentWeightString, currentSize + sizeof(char));
          currentSize += sizeof(char);
          j++;
        }
        break;
      }
    }
    currentWeight = atoi(currentWeightString);

    // Writing the camps from the container struct
    containerList[i]->code = (char *)malloc(strlen(currentCode) + 1);
    containerList[i]->cnpj = (char *)malloc(strlen(currentCnpj) + 1);

    strcpy(containerList[i]->code, currentCode);
    strcpy(containerList[i]->cnpj, currentCnpj);
    containerList[i]->weight = currentWeight;
    i++;
  }
}

int main() {
  Container *containers[20];
  int containersListSize = sizeof(containers) / sizeof(containers[0]), i = 0;

  for (i = 0; i < 20; i++) {
    containers[i] = (Container *)malloc(sizeof(Container));
    containers[i]->code = NULL;
    containers[i]->cnpj = NULL;
  }

  printf("Eu sou");

  readInputAndCreateContainerList(containers);

  for (int i = 0; i < 11; i++) {
    printf("%s | %s | %d", containers[i]->code, containers[i]->cnpj,
           containers[i]->weight);
    printf("\n");
  }
  return EXIT_SUCCESS;
}

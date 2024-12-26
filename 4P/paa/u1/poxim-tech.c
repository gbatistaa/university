#include <stdio.h>
#include <stdlib.h>

typedef struct container {
  char *code;
  char *cnpj;
  float weight;
} Container;

void merge(int array[], int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  int leftSubArray[n1], rightSubArray[n2];

  for (int i = 0; i < n1; i++)
    leftSubArray[i] = array[left + i];
  for (int j = 0; j < n2; j++)
    rightSubArray[j] = array[mid + 1 + j];

  int i = 0;
  int j = 0;
  int k = left;

  while (i < n1 && j < n2) {
    if (leftSubArray[i] <= rightSubArray[j]) {
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

void mergeSort(int arr[], int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    merge(arr, left, mid, right);
  }
}

int main() {
  printf("");
  return EXIT_SUCCESS;
}

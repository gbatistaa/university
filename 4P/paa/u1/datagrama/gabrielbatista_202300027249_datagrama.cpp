#include <cstdlib>
#include <iostream>

using namespace std;

int heapify(int array[], int size, int root) {
  int bigger = root;
  int left = (2 * root) + 1;
  int right = (2 * root) + 2;

  if (left < size && array[left] > array[bigger]) {
    bigger = left;
  }

  if (right < size && array[right] > array[bigger]) {
    bigger = right;
  }

  if (bigger != root) {
    swap(array[root], array[bigger]);
    heapify(array, size, bigger);
  }

  return EXIT_SUCCESS;
}

/*
  - O heap começa do primeiro nó não folha até a raiz
  - Por isso deve-se iterar do índice desse número até 0 (raiz)
*/

int buildHeap(int array[], int size) {
  for (int i = (size / 2) - 1; i >= 0; i--) {
    heapify(array, size, i);
  }
  return EXIT_SUCCESS;
}

int main() {
  int array[6] = {42, 23, 95, 19, 56, 33};
  int size = sizeof(array) / sizeof(array[0]);

  buildHeap(array, size);

  cout << "Max-Heap: ";
  for (int i = 0; i < size; i++) {
    cout << array[i] << " ";
  }
  cout << endl;

  return EXIT_SUCCESS;
}

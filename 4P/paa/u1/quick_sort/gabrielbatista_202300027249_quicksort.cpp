#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"

using namespace std;
using namespace std::chrono;

// Class for monitoring the matrix and its vectors and the size of each one
enum Particioning { LP, LM, LA, HP, HM, HA };

class Matrix {
public:
  int size;
  int *sizes;
  int **list;
};

class Quick_Variation {
public:
  string message;
  Particioning code;
  int calls;
};

int lomuto(int *vector, int start, int end) {
  int pivot = vector[end];
  int x = start - 1;
  int y = start;
  for (y = start; y < end; y++) {
    if (vector[y] <= pivot) {
      swap(vector[++x], vector[y]);
    }
  }
  swap(vector[++x], vector[end]);
  return x;
}

int lomuto_median(int *vector, int start, int end) {
  int mid = start + (end - start) / 2;
  int median_index = (vector[start] <= vector[mid])
                         ? ((vector[mid] <= vector[end])
                                ? mid
                                : (vector[start] <= vector[end] ? end : start))
                         : ((vector[start] <= vector[end])
                                ? start
                                : (vector[mid] <= vector[end] ? end : mid));
  int temp = vector[end];
  vector[end] = vector[median_index];
  vector[median_index] = temp;
  int pivot = vector[end];
  int i = start - 1;
  for (int j = start; j < end; j++) {
    if (vector[j] <= pivot) {
      temp = vector[++i];
      vector[i] = vector[j];
      vector[j] = temp;
    }
  }
  temp = vector[i + 1];
  vector[i + 1] = vector[end];
  vector[end] = temp;

  return i + 1;
}

int lomuto_random(int *vector, int start, int end) {
  int index = start + rand() % (end - start + 1);
  swap(vector[end], vector[index]);
  return lomuto(vector, start, end);
}

int hoare(int *vector, int start, int end) {
  int pivot = vector[start], x = start - 1, y = end + 1;
  while (true) {
    while (vector[--y] > pivot)
      ;
    while (vector[++x] < pivot)
      ;
    if (x < y)
      swap(vector[x], vector[y]);
    else
      return y;
  }
}

int hoare_median(int *vector, int start, int end) {
  // Calcula o índice da mediana entre o primeiro, o meio e o último elementos
  int mid = start + (end - start) / 2;
  int median_index = (vector[start] <= vector[mid])
                         ? ((vector[mid] <= vector[end])
                                ? mid
                                : (vector[start] <= vector[end] ? end : start))
                         : ((vector[start] <= vector[end])
                                ? start
                                : (vector[mid] <= vector[end] ? end : mid));

  int pivot = vector[median_index];
  int temp = vector[start];
  vector[start] = vector[median_index];
  vector[median_index] = temp;
  int i = start - 1;
  int j = end + 1;
  while (1) {
    do {
      i++;
    } while (vector[i] < pivot);
    do {
      j--;
    } while (vector[j] > pivot);
    if (i >= j) {
      return j;
    }
    temp = vector[i];
    vector[i] = vector[j];
    vector[j] = temp;
  }
}

int hoare_random(int *vector, int start, int end) {
  int index = start + rand() % (end - start + 1);
  swap(vector[end], vector[index]);
  return hoare(vector, start, end);
}

int pivot_chooser(int *vector, int start, int end, Particioning code) {
  int pivot;
  switch (code) {
  case LP:
    pivot = lomuto(vector, start, end);
    break;
  case LM:
    pivot = lomuto_median(vector, start, end);
    break;
  case LA:
    pivot = lomuto_random(vector, start, end);
    break;
  case HP:
    pivot = hoare(vector, start, end);
    break;
  case HM:
    pivot = hoare_median(vector, start, end);
    break;
  case HA:
    pivot = hoare_random(vector, start, end);
    break;
  }
  return pivot;
}

int quick_sort(int *vector, int start, int end, Particioning code) {
  if (start < end) {
    int pivot = pivot_chooser(vector, start, end, code);
    quick_sort(vector, start, code < HP ? pivot - 1 : pivot, code);
    quick_sort(vector, pivot + 1, end, code);
  }
  return EXIT_SUCCESS;
}

int read_input(ifstream &input, Matrix *&vectors) {
  string line;

  // Reading the first line and get the vectors quantity
  if (!getline(input, line)) {
    cerr << "Erro ao ler linha" << endl;
    return EXIT_FAILURE;
  }

  int vectors_quantity = stoi(line);

  // Creating the vectors list with pre allocation size and list for the matrix
  // vectors sizes
  vectors->list = new int *[vectors_quantity];
  vectors->sizes = new int[vectors_quantity];
  vectors->size = vectors_quantity;

  // Reading the vectors lines
  int i = 0;
  bool is_size_line = true;
  while (getline(input, line)) {

    // Pre allocating the current vector with its obtained size
    if (is_size_line) {
      int vector_size = stoi(line);
      vectors->list[i] = new int[vector_size];
      is_size_line = false;
    } else {
      int element = 0, j = 0;

      // Storing each vector elements on its vector
      istringstream iss(line);
      while (iss >> element) {
        vectors->list[i][j++] = element;
      }
      vectors->sizes[i++] = j;
      is_size_line = true;
    }
  }

  return EXIT_SUCCESS;
}

bool sorted_it_is(int *vector, int size) {
  if (size <= 1)
    return true;
  for (int i = 1; i < size; i++)
    if (vector[i] < vector[i - 1])
      return false;
  return true;
}

int main(int argc, char *argv[3]) {
  auto start_program = high_resolution_clock::now();

  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "erro ao abrir input" << endl;
    return EXIT_FAILURE;
  }

  cout << "Input aberto com sucesso!\n" << endl;

  if (!output.is_open()) {
    cerr << "erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }

  cout << "Output aberto com sucesso!\n" << endl;

  Matrix *vectors = new Matrix();

  read_input(input, vectors);

  for (int i = 0; i < vectors->size; i++) {
    quick_sort(vectors->list[i], 0, vectors->sizes[i] - 1, HA);
    cout << (sorted_it_is(vectors->list[i], vectors->sizes[i])
                 ? GREEN "Está ordenado" RESET
                 : RED "Não está ordenado" RESET)
         << endl;
  }

  input.close();
  output.close();

  auto end_program = high_resolution_clock::now(); // Fim do programa

  cout << "Tempo total do programa: "
       << duration<float>(end_program - start_program).count() << " s\n";

  return EXIT_SUCCESS;
}

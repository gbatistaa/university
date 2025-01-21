#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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
  string format;
  Particioning code;
  int calls;
};

int lomuto(int *vector, int start, int end) {
  int pivot = vector[end];
  return pivot;
}

// int lomuto_median(int &vector, int start, int end) {}
// int lomuto_random(int &vector, int start, int end) {}

// int hoare(int &vector, int start, int end) {}
// int hoare_median(int &vector, int start, int end) {}
// int hoare_random(int &vector, int start, int end) {}

int pivot_chooser(int *vector, Particioning code) {
  int pivot = 0;
  switch (code) {
  case LP:
    break;
  case LM:
    break;
  case LA:
    break;
  case HP:
    break;
  case HM:
    break;
  case HA:
    break;
  }
  return pivot;
}

int quick_sort(int *vector, int start, int end, Particioning code) {
  if (start < end) {
    int pivot = pivot_chooser(vector, code);
    quick_sort(vector, start, pivot, code);
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
      vectors->sizes[i++] = j + 1;
      is_size_line = true;
    }
  }

  return EXIT_SUCCESS;
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

  input.close();
  output.close();

  auto end_program = high_resolution_clock::now(); // Fim do programa

  cout << "Tempo total do programa: "
       << duration<float>(end_program - start_program).count() << " s\n";

  return EXIT_SUCCESS;
}

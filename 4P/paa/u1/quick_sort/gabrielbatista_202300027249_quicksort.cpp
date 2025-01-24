#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

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
  int calls;

  // Quick variation constructor
  Quick_Variation() : message(""), calls(0) {}
};

void swap(int &n1, int &n2) {
  int aux = n1;
  n1 = n2;
  n2 = aux;
}

int lomuto(int *vector, int start, int end, int &calls) {
  int pivot = vector[end], x = start - 1;
  for (int y = start; y < end; y++) {
    if (vector[y] <= pivot) {
      swap(vector[++x], vector[y]);
      calls++;
    }
  }
  swap(vector[++x], vector[end]);
  calls++;
  return x;
}

int lomuto_median(int *vector, int start, int end, int &calls) {
  int size = end - start + 1;
  int i1 = start + (size / 4), i2 = start + (size / 2),
      i3 = start + ((3 * size) / 4);
  if ((vector[i1] >= vector[i2] && vector[i1] <= vector[i3]) ||
      (vector[i1] <= vector[i2] && vector[i1] >= vector[i3])) {
    swap(vector[end], vector[i1]);
    calls++;
    return lomuto(vector, start, end, calls);
  }
  if ((vector[i2] >= vector[i1] && vector[i2] <= vector[i3]) ||
      (vector[i2] <= vector[i1] && vector[i2] >= vector[i3])) {
    swap(vector[end], vector[i2]);
    calls++;
    return lomuto(vector, start, end, calls);
  }
  swap(vector[end], vector[i3]);
  calls++;
  return lomuto(vector, start, end, calls);
}

int lomuto_random(int *vector, int start, int end, int &calls) {
  int size_sub_vec = end - start + 1;
  int random_idx = start + abs(vector[start]) % size_sub_vec;
  swap(vector[random_idx], vector[end]);
  calls++;
  return lomuto(vector, start, end, calls);
}

int hoare(int *vector, int start, int end, int &calls) {
  int pivot = vector[start], x = start - 1, y = end + 1;
  while (true) {
    while (vector[--y] > pivot)
      ;
    while (vector[++x] < pivot)
      ;
    if (x < y) {
      swap(vector[x], vector[y]);
      calls++;
    } else
      return y;
  }
}

int hoare_median(int *vector, int start, int end, int &calls) {
  int size = end - start + 1;
  int i1 = start + (size / 4), i2 = start + (size / 2),
      i3 = start + ((3 * size) / 4);
  if ((vector[i1] >= vector[i2] && vector[i1] <= vector[i3]) ||
      (vector[i1] <= vector[i2] && vector[i1] >= vector[i3])) {
    swap(vector[end], vector[i1]);
    calls++;
    return hoare(vector, start, end, calls);
  }
  if ((vector[i2] >= vector[i1] && vector[i2] <= vector[i3]) ||
      (vector[i2] <= vector[i1] && vector[i2] >= vector[i3])) {
    swap(vector[end], vector[i2]);
    calls++;
    return hoare(vector, start, end, calls);
  }
  swap(vector[end], vector[i3]);
  calls++;
  return hoare(vector, start, end, calls);
}

int hoare_random(int *vector, int start, int end, int &calls) {
  int size_sub_vec = end - start + 1;
  int random_idx = start + abs(vector[start]) % size_sub_vec;
  swap(vector[random_idx], vector[start]);
  calls++;
  return hoare(vector, start, end, calls);
}

int pivot_chooser(int *vector, int start, int end, Particioning code,
                  int &calls) {
  int pivot = 0;
  switch (code) {
  case LP:
    pivot = lomuto(vector, start, end, calls);
    break;
  case LM:
    pivot = lomuto_median(vector, start, end, calls);
    break;
  case LA:
    pivot = lomuto_random(vector, start, end, calls);
    break;
  case HP:
    pivot = hoare(vector, start, end, calls);
    break;
  case HM:
    pivot = hoare_median(vector, start, end, calls);
    break;
  case HA:
    pivot = hoare_random(vector, start, end, calls);
    break;
  }
  return pivot;
}

int quick_sort(int *vector, int start, int end, Particioning code, int &calls) {
  if (start < end) {
    int pivot = pivot_chooser(vector, start, end, code, calls);
    quick_sort(vector, start, code < HP ? pivot - 1 : pivot, code, calls);
    calls++;
    quick_sort(vector, pivot + 1, end, code, calls);
    calls++;
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

  string names[6] = {"LP", "LM", "LA", "HP", "HM", "HA"};

  for (int i = 0; i < vectors->size; i++) {
    int stable_vector[vectors->sizes[i]];
    Quick_Variation variations[6] = {};
    string vector_id =
        to_string(i) + ":N(" + to_string(vectors->sizes[i]) + ")";
    cout << vector_id;
    int calls = 0;
    for (int part = LP; part <= HA; part++) {
      for (int j = 0; j < vectors->sizes[i]; j++) {
        stable_vector[j] = vectors->list[i][j];
      }
      calls++;
      quick_sort(stable_vector, 0, vectors->sizes[i] - 1, (Particioning)part,
                 calls);
      variations[part].message =
          "," + names[part] + "(" + to_string(calls) + ")";
      variations[part].calls = calls;
      calls = 0;
    }
    for (int part = LP; part <= HA; part++) {
      cout << variations[part].message;
    }
    cout << endl;
  }

  input.close();
  output.close();

  auto end_program = high_resolution_clock::now();

  cout << "\nExecution time: "
       << duration<float>(end_program - start_program).count() << " s\n";

  return EXIT_SUCCESS;
}

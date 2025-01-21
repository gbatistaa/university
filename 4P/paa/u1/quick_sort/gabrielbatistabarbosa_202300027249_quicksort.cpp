#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// Class for monitoring the matrix and its vectors and the size of each one
class Matrix {
public:
  int *sizes;
  int **list;
};

int read_input(ifstream &input, Matrix *&vectors) {
  string line;

  // Reading the first line and get the vectors quantity
  if (!getline(input, line)) {
    cerr << "Erro ao ler linha" << endl;
    return EXIT_FAILURE;
  }

  cout << "Extraindo dados do arquivo..." << endl;

  int vectors_quantity = stoi(line);
  cout << "Serão " << vectors_quantity << " vetores" << endl;

  // Creating the vectors list with pre allocation size
  vectors->list = new int *[vectors_quantity];
  vectors->sizes = new int[vectors_quantity];

  // Reading the vectors lines
  int i = 0;
  bool is_size_line = true;
  while (getline(input, line)) {

    // Pre allocating the current vector with its obtained size
    if (is_size_line) {
      int vector_size = stoi(line);
      vectors->list[i] = new int[vector_size];
      cout << "Criado vetor " << i + 1
           << " com " + line + " elementos no endereço: " << vectors->list[i]
           << endl;
      is_size_line = false;
    } else {
      int element = 0, j = 0;

      // Storing each vector elements on its vector
      istringstream iss(line);
      cout << "Elementos do vetor " << i + 1 << ": ";
      while (iss >> element) {
        cout << "(" << element << "| " << j << ") ";
        vectors->list[i][j] = element;
        j++;
      }
      cout << endl;

      i++;
      is_size_line = true;
    }
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[3]) {
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

  return EXIT_SUCCESS;
}

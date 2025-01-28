#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class Package {
  int code;
  int size;
  char *bytes_list[2];
};

class PackageList {
public:
  int total_pkgs;
  int pkgs_per_read;
  Package *pkgs;
};

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

int build_heap(int array[], int size) {
  for (int i = (size / 2) - 1; i >= 0; i--) {
    heapify(array, size, i);
  }
  return EXIT_SUCCESS;
}

int read_file(ifstream &input) {
  string line;

  // Reading ther first line and getting the pkgs total and pkgs quantity
  if (!getline(input, line)) {
    cerr << "Error on the file reading" << endl;
  }

  istringstream iss(line);

  return EXIT_SUCCESS;
}

int main(int args, char *argv[3]) {
  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "Erro ao abrir input" << endl;
    return EXIT_FAILURE;
  }

  cout << "Input aberto com sucesso!\n" << endl;

  if (!output.is_open()) {
    cerr << "Erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }

  PackageList package_list = PackageList();

  read_file(input);

  cout << "Output aberto com sucesso!\n" << endl;

  return EXIT_SUCCESS;
}

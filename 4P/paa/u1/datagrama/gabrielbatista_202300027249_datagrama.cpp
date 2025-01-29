#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#define MAX_SIZE 512

using namespace std;

class Package {
public:
  int code;
  int size;
  string *bytes_list;
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

int read_file(ifstream &input, PackageList *&pkg_list) {

  // Reading ther first line and getting the total pkgs and pkgs per read:

  string line;
  getline(input, line);
  istringstream iss(line);
  iss >> pkg_list->total_pkgs;
  iss >> pkg_list->pkgs_per_read;

  // Pre allocating the packages list with its obtained size
  pkg_list->pkgs = new Package[pkg_list->total_pkgs];

  int i = 0;
  while (getline(input, line)) {

    // Storing each package infos
    istringstream iss(line);
    iss >> pkg_list->pkgs[i].code;
    iss >> pkg_list->pkgs[i].size;

    // Preallocating the bytes list with its obtained size:
    pkg_list->pkgs[i].bytes_list = new string[pkg_list->pkgs[i].size];

    // Reading all the bytes and storing in the bytes list:
    int j = 0;
    string byte;
    while (iss >> byte) {
      pkg_list->pkgs[i].bytes_list[j++] = byte;
    }
    i++;
  }
  return EXIT_SUCCESS;
}

int print_sorted_pkgs(Package *pkgs, int list_size, int pkgs_per_read) {
  int last_pkg = 0, expected_pkg = 0, curr_pkg = 0, wait_index = 0;
  bool is_gonna_wait = false;
  Package wait_list[list_size];
  while (expected_pkg < list_size - 1) {

    // reading the block of n elements:
    for (int i = 0; i < pkgs_per_read || expected_pkg < list_size; i++) {
      if (pkgs[curr_pkg].code == expected_pkg) {
        cout << pkgs[curr_pkg].code << " ";
        expected_pkg++;
      } else {
        wait_list[wait_index++] = pkgs[curr_pkg];
      }
    }
    wait_index = 0;
  }
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
  cout << "Output aberto com sucesso!" << endl;

  PackageList *package_list = new PackageList();

  read_file(input, package_list);

  cout << "Pacotes por leitura: " << package_list->pkgs_per_read << endl;
  cout << "Pacotes totais: " << package_list->total_pkgs << endl;

  for (int i = 0; i < package_list->total_pkgs; i++) {
    cout << package_list->pkgs[i].code << " | " << package_list->pkgs[i].size
         << ": ";
    for (int j = 0; j < package_list->pkgs[i].size; j++) {
      cout << package_list->pkgs[i].bytes_list[j] << " ";
    }
    cout << endl;
  }
  cout << endl;

  print_sorted_pkgs(package_list->pkgs, package_list->total_pkgs,
                    package_list->pkgs_per_read);

  return EXIT_SUCCESS;
}

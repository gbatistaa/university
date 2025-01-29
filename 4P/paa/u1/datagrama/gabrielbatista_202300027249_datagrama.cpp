#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define MAX_SIZE 512

using namespace std;

class Package {
public:
  int code = 0;
  int size = 0;
  string *bytes_list = nullptr;
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

int print_pkg_bytes(Package pkg) {
  cout << "|";
  for (int i = 0; i < pkg.size - 1; i++) {
    cout << pkg.bytes_list[i] << ",";
  }
  cout << pkg.bytes_list[pkg.size - 1] << endl;
  return EXIT_SUCCESS;
}

int write_sorted_pkgs(Package *pkgs, int list_size, int pkgs_per_read) {
  int expected_pkg = 0, curr_pkg = 0, wait_start = 0, wait_end = -1;
  Package wait_list[list_size];
  while (expected_pkg < list_size - 1) {

    // reading the block of n elements and storing in the wait list:
    for (int i = 0; i < pkgs_per_read || expected_pkg < list_size; i++) {
      wait_list[wait_end++] = pkgs[curr_pkg++];
    }

    // heapify packages...

    // Searching the expected package in order:
    int curr_wait = 0;
    for (int j = 0; j < pkgs_per_read || expected_pkg < list_size; j++) {

      // Condition triggered when the expected package is found:
      if (wait_list[curr_wait].code == expected_pkg) {
        print_pkg_bytes(wait_list[curr_wait]);
        wait_list[curr_wait++] =
            Package(); // Erasing the found in-order package
        expected_pkg++;
      } else {
        cout << "\n";
        break;
      }
    }
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
  cout << "Output aberto com sucesso!\n" << endl;

  PackageList *package_list = new PackageList();

  read_file(input, package_list);

  cout << "Pacotes por leitura: " << package_list->pkgs_per_read << endl;
  cout << "Pacotes totais: " << package_list->total_pkgs << endl;

  // print_pkg_bytes(package_list->pkgs[4]);
  write_sorted_pkgs(package_list->pkgs, package_list->total_pkgs,
                    package_list->pkgs_per_read);

  cout << "rodou" << endl;

  return EXIT_SUCCESS;
}

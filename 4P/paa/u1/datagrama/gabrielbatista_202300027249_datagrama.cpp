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

int swap(Package &n1, Package &n2) {
  Package aux = n1;
  n1 = n2;
  n2 = aux;

  return EXIT_SUCCESS;
}

int max_heapify(Package array[], int size, int root) {
  int largest = root;
  int left = 2 * root + 1;
  int right = 2 * root + 2;

  if (left < size && array[left].code > array[largest].code) {
    largest = left;
  }

  if (right < size && array[right].code > array[largest].code) {
    largest = right;
  }

  if (largest != root) {
    swap(array[root], array[largest]);
    max_heapify(array, size, largest);
  }
  return EXIT_SUCCESS;
}

int build_max_heap(Package array[], int size) {
  for (int i = size / 2 - 1; i >= 0; i--) {
    max_heapify(array, size, i);
  }
  return EXIT_SUCCESS;
}

int heap_sort(Package array[], int size) {
  build_max_heap(array, size);

  for (int i = size - 1; i > 0; i--) {
    swap(array[0], array[i]);
    max_heapify(array, i, 0);
  }
  return EXIT_SUCCESS;
}

int write_pkg_bytes(ofstream &output, Package pkg) {
  output << "|";
  for (int i = 0; i < pkg.size - 1; i++) {
    output << pkg.bytes_list[i] << ",";
  }
  output << pkg.bytes_list[pkg.size - 1];
  return EXIT_SUCCESS;
}

int write_sorted_pkgs(ofstream &output, Package *pkgs, int list_size,
                      int pkgs_per_read) {
  int expected_pkg = 0, wait_end = -1, read_pkgs = 0;
  Package wait_list[list_size];
  for (int i = 0; i < list_size; i++) {
    read_pkgs++;
    wait_list[++wait_end] = pkgs[i];
    if (read_pkgs % pkgs_per_read == 0) {
      if (read_pkgs > pkgs_per_read)
        output << "\n";
      for (int j = 0; j <= wait_end; j++) {
        heap_sort(wait_list, wait_end + 1);
        if (wait_list[j].code == expected_pkg) {
          write_pkg_bytes(output, wait_list[j]);
          expected_pkg++;
        }
      }
      output << "|";
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

  if (!output.is_open()) {
    cerr << "Erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }

  PackageList *package_list = new PackageList();

  read_file(input, package_list);

  write_sorted_pkgs(output, package_list->pkgs, package_list->total_pkgs,
                    package_list->pkgs_per_read);
  return EXIT_SUCCESS;
}

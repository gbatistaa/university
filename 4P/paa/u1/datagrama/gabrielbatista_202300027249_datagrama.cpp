#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace std::chrono;

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

int hoare(Package *array, int low, int high) {
  int pivot = array[(low + high) / 2].code;
  int i = low - 1;
  int j = high + 1;
  while (true) {
    while (array[++i].code < pivot)
      ;
    while (array[--j].code > pivot)
      ;
    if (i >= j)
      return j;
    swap(array[i], array[j]);
  }
}

int quick_sort(Package *array, int low, int high) {
  if (low < high) {
    int partition_index = hoare(array, low, high);
    quick_sort(array, low, partition_index);
    quick_sort(array, partition_index + 1, high);
  }
  return EXIT_SUCCESS;
}

int write_pkg_bytes(Package pkg, string &output_str) {
  output_str += "|";
  for (int i = 0; i < pkg.size - 1; i++) {
    output_str += pkg.bytes_list[i];
    output_str += ",";
  }
  output_str += pkg.bytes_list[pkg.size - 1];
  return EXIT_SUCCESS;
}

int write_sorted_pkgs(ofstream &output, Package *pkgs, int list_size,
                      int pkgs_per_read) {
  Package wait_list[list_size];
  string output_string = "";
  int expected_pkg = 0, wait_end = -1, read_pkgs = 0;
  for (int i = 0; i < list_size; i++) {
    read_pkgs++;
    wait_list[++wait_end] = pkgs[i];
    if (read_pkgs % pkgs_per_read == 0) {
      bool is_something_written = false;
      quick_sort(wait_list, 0, wait_end);
      for (int j = 0; j <= wait_end; j++) {
        if (wait_list[j].code == expected_pkg) {
          is_something_written = true;
          write_pkg_bytes(wait_list[j], output_string);
          expected_pkg++;
        }
      }
      is_something_written ? output_string += "|\n" : output_string += "";
    }
  }
  output << output_string;
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
  auto start = high_resolution_clock::now();

  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "Error on input file opening" << endl;
    return EXIT_FAILURE;
  }

  if (!output.is_open()) {
    cerr << "Error on output file opening" << endl;
    return EXIT_FAILURE;
  }

  PackageList *package_list = new PackageList();

  read_file(input, package_list);

  write_sorted_pkgs(output, package_list->pkgs, package_list->total_pkgs,
                    package_list->pkgs_per_read);

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;
  cout << "Execution time: " << duration.count() << " s" << endl;

  return EXIT_SUCCESS;
}

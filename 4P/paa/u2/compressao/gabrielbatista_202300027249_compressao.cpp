#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

class Data {
public:
  string *bytes = nullptr;
  int size = 0;
};

class DataList {
public:
  Data *datas = nullptr;
  int size = 0;
};

double getMemoryUsageMB() {
  ifstream fp("/proc/self/statm");
  long rss = 0L;
  string dummy;
  if (fp) {
    fp >> dummy >> rss;
  }
  return rss * sysconf(_SC_PAGESIZE) / (1024.0 * 1024.0);
}

int read_file(ifstream &input, DataList *&data_list) {
  string line = "";

  // Allocating memoy for the datas list:
  getline(input, line);
  int data_list_size = stoi(line);
  data_list->size = data_list_size;
  data_list->datas = new Data[data_list_size];

  // Allocating memory for each data set:
  for (int i = 0; getline(input, line); i++) {
    istringstream iss(line);
    iss >> data_list->datas[i].size;
    data_list->datas[i].bytes = new string[data_list->datas[i].size];

    int j = 0;
    while (iss >> data_list->datas[i].bytes[j]) {
      j++;
    }
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

  double ram_before = getMemoryUsageMB();
  auto start = high_resolution_clock::now();

  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "Erro ao abrir input" << endl;
    return EXIT_FAILURE;
  }
  cout << "Input aberto com sucesso!" << endl;

  if (!output.is_open()) {
    cerr << "Erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }
  cout << "Output aberto com sucesso!\n" << endl;

  DataList *data_list = new DataList();

  read_file(input, data_list);

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "Execution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}

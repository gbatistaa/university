#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
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

string RLE(Data data) {
  int qty = 1;
  string curr_byte = data.bytes[0], data_compress = "";
  for (int i = 1; i < data.size; i++) {
    if (data.bytes[i] == data.bytes[i - 1]) {
      qty++;
    } else {
      stringstream ss;
      ss << hex << setw(2) << setfill('0') << qty;
      data_compress += ss.str() + curr_byte;
      qty = 1;
      curr_byte = data.bytes[i];
    }
  }
  stringstream ss;
  ss << hex << setw(2) << setfill('0') << qty;
  data_compress += ss.str() + curr_byte;

  return data_compress;
}

int read_file(ifstream &input, DataList *&data_list) {
  string line = "";

  // Aloca memória para a lista de dados:
  getline(input, line);
  int data_list_size = stoi(line);
  data_list->size = data_list_size;
  data_list->datas = new Data[data_list_size];

  // Aloca memória para cada conjunto de dados:
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

  string output_string = "";
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

  for (int i = 0; i < data_list->size; i++) {
    string compress_rle = RLE(data_list->datas[i]);
    output_string += to_string(i) + "->RLE=" + compress_rle + "\n";
  }

  output << output_string;

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "Execution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}

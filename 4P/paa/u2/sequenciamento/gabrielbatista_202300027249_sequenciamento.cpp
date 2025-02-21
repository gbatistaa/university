#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h> // Para sysconf() no Linux

using namespace std;
using namespace std::chrono;

class DNA {
public:
  int sub_string_size = 0;
  string dna_sequence = "";
};

class Desease {
public:
  string code = "";
  string *genes = nullptr;
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

// float calculate_desease_chance(string dna_sequence, string *desease_genes,
//                                int sub_string_size) {
//                                }

int process_desease(string &output_string, string desease_line, DNA *dna,
                    Desease *&deseases, int i) {
  istringstream iss(desease_line);
  int genes_qty = 0;

  iss >> deseases[i].code;
  output_string += deseases[i].code + "->";

  iss >> genes_qty;

  deseases[i].genes = new string[genes_qty];

  for (int g = 0; g < genes_qty; g++) {
    iss >> deseases[i].genes[g];
  }

  output_string += "\n";

  return EXIT_SUCCESS;
}

int read_file(ifstream &input, string &output_string, DNA *&dna) {
  string line;

  // Reading the DNA string data:
  getline(input, line);
  dna->sub_string_size = stoi(line);
  getline(input, dna->dna_sequence);

  // Reading the diseases quantity:
  int deseases_qty;
  getline(input, line);
  deseases_qty = stoi(line);

  // Allocating memory for the diseases list:
  Desease *deseases = new Desease[deseases_qty];

  // Processing all of the diseases and their genes:
  for (int i = 0; getline(input, line); i++) {
    process_desease(output_string, line, dna, deseases, i);
  }

  delete[] deseases;

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
  cout << "Output aberto com sucesso!" << endl;

  string output_string = "";

  DNA *dna = new DNA();

  read_file(input, output_string, dna);

  delete dna;

  output << output_string;

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "\nExecution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}

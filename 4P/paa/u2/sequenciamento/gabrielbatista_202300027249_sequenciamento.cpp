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

// Função para medir uso de RAM em MB no Linux
double getMemoryUsageMB() {
  long rss = 0L;
  FILE *fp = fopen("/proc/self/statm", "r");
  if (fp) {
    fscanf(fp, "%*s%ld", &rss);
    fclose(fp);
  }
  return rss * sysconf(_SC_PAGESIZE) / (1024.0 * 1024.0);
}

int process_desease(string &output_string, string desease_line, DNA *dna,
                    Desease *&deseases, int i) {
  istringstream iss(desease_line);
  int genes_qty = 0;

  Desease *desease = new Desease();
  iss >> desease->code;
  iss >> genes_qty;
  desease->genes = new string[genes_qty];

  output_string += desease->code + "->\n";

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

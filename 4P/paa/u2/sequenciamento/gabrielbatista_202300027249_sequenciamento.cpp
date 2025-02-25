#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>

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
  int desease_chance = 0;
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

int insert(int *R, int pos, int &index) {
  R[index++] = pos;
  return EXIT_SUCCESS;
}

int calculate_table(int *k, string standard) {
  int m = standard.length();
  if (m == 0)
    return EXIT_SUCCESS;

  k[0] = -1;
  for (int i = 1, j = -1; i < m; i++) {
    while (j >= 0 && standard.at(j + 1) != standard.at(i)) {
      j = k[j];
    }
    if (j + 1 < m && standard.at(j + 1) == standard.at(i)) {
      j++;
    }
    k[i] = j;
  }
  return EXIT_SUCCESS;
}

int KMP(int *k, int *R, string str, string std, int minimal_substring_size,
        int &oc_num, int &minimal_size_ocurrencies) {
  int n = str.length();
  int m = std.length();
  if (m == 0 || n == 0)
    return EXIT_SUCCESS;
  string cs = "";

  calculate_table(k, std);
  int index = 0;
  for (int i = 0, j = -1; i < n; i++) {
    while (j >= 0 && (j + 1 >= m || std[j + 1] != str[i])) {
      cout << oc_num << " ";
      if (oc_num >= minimal_substring_size) {
        minimal_size_ocurrencies += oc_num;
      }
      oc_num = 0;
      j = k[j];
    }
    if (j + 1 < m && std[j + 1] == str[i]) {
      j++, oc_num++;
    }
    if (j == m - 1) {
      insert(R, i - m + 1, index);
      cout << oc_num << " ";
      if (oc_num >= minimal_substring_size) {
        minimal_size_ocurrencies += oc_num;
      }
      cout << "|Encontrou ";
      oc_num = 0;
      j = k[j];
    }
  }
  return EXIT_SUCCESS;
}

float calculate_desease_chance(string dna_sequence, string *desease_genes,
                               string &output_string, int sub_string_size,
                               int genes_qty) {
  int probable_genes = 0;
  // Iterating on the all genes list of the desease:
  for (int i = 0; i < genes_qty; i++) {
    int gene_length = desease_genes[i].length(),
        dna_size = dna_sequence.length(), ocurrences_num = 0,
        biggest_sequence_lenght = 0, minimal_size_ocurrencies = 0;
    cout << desease_genes[i] + "->";

    int *k = new int[gene_length];
    int *ocurrences = new int[gene_length];
    KMP(k, ocurrences, dna_sequence, desease_genes[i], sub_string_size,
        ocurrences_num, minimal_size_ocurrencies);

    cout << endl;
    if (minimal_size_ocurrencies >= sub_string_size)
      probable_genes++;
  }
  float desease_chance = ceil(((float)probable_genes / genes_qty) * 100);
  output_string += to_string((int)desease_chance) + "%";
  return desease_chance;
}

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

  calculate_desease_chance(dna->dna_sequence, deseases[i].genes, output_string,
                           dna->sub_string_size, genes_qty);

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

  cout << dna->dna_sequence << endl;
  cout << endl;

  for (int i = 0; getline(input, line); i++) {
    process_desease(output_string, line, dna, deseases, i);
    cout << endl;
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
  cout << "Output aberto com sucesso!\n" << endl;

  string output_string = "";

  DNA *dna = new DNA();

  read_file(input, output_string, dna);

  delete dna;

  output << output_string;

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "Execution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

class DNA {
public:
  int sub_string_size = 0;
  string dna_sequence = "";
};

class Disease {
public:
  string code = "";
  string *genes = nullptr;
  int disease_chance = 0;
};

struct DiseaseWithIndex {
  Disease *disease;
  int original_index;
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

int partition(DiseaseWithIndex *arr, int low, int high) {
  int pivot = arr[high].disease->disease_chance; // Pivô é o último elemento
  int i = low - 1;

  for (int j = low; j < high; j++) {
    // Ordena em ordem decrescente com base em disease_chance
    // Se disease_chance for igual, usa o índice original para manter a
    // estabilidade
    if (arr[j].disease->disease_chance > pivot ||
        (arr[j].disease->disease_chance == pivot &&
         arr[j].original_index < arr[high].original_index)) {
      i++;
      swap(arr[i], arr[j]);
    }
  }
  swap(arr[i + 1], arr[high]); // Coloca o pivô na posição correta
  return i + 1;
}

// Função recursiva do Quick Sort
void quickSortStable(DiseaseWithIndex *arr, int low, int high) {
  if (low < high) {
    int pi = partition(arr, low, high); // Índice do pivô
    quickSortStable(arr, low, pi - 1);  // Ordena a sublista à esquerda do pivô
    quickSortStable(arr, pi + 1, high); // Ordena a sublista à direita do pivô
  }
}

// Função para ordenar um vetor de Disease com base em disease_chance
void sortDiseases(Disease *diseases, int n) {
  // Cria um vetor auxiliar com Disease e índices originais
  DiseaseWithIndex *arr = new DiseaseWithIndex[n];
  for (int i = 0; i < n; i++) {
    arr[i].disease = &diseases[i];
    arr[i].original_index = i;
  }

  // Aplica o Quick Sort estável
  quickSortStable(arr, 0, n - 1);

  // Cria um vetor temporário para armazenar o resultado ordenado
  Disease *temp = new Disease[n];
  for (int i = 0; i < n; i++) {
    temp[i] = *arr[i].disease;
  }

  // Copia o resultado de volta para o vetor original
  for (int i = 0; i < n; i++) {
    diseases[i] = temp[i];
  }
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
      if (oc_num >= minimal_substring_size) {
        minimal_size_ocurrencies += oc_num;
      }
      oc_num = 0;
      j = k[j];
    }
  }
  return EXIT_SUCCESS;
}

int calculate_disease_chance(string dna_sequence, string *disease_genes,
                             string &output_string, int sub_string_size,
                             int genes_qty) {
  int probable_genes = 0;
  // Iterating on the all genes list of the desease:
  for (int i = 0; i < genes_qty; i++) {
    int gene_length = disease_genes[i].length(), ocurrences_num = 0,
        minimal_size_ocurrencies = 0;

    int *k = new int[gene_length];
    int *ocurrences = new int[gene_length];
    KMP(k, ocurrences, dna_sequence, disease_genes[i], sub_string_size,
        ocurrences_num, minimal_size_ocurrencies);

    if (minimal_size_ocurrencies >= sub_string_size)
      probable_genes++;
  }
  int disease_chance = round(((float)probable_genes / genes_qty) * 100);
  disease_chance = disease_chance >= 90 ? 100 : disease_chance;
  return disease_chance;
}

int process_disease(string &output_string, string disease_line, DNA *dna,
                    Disease *diseases, int i) {
  istringstream iss(disease_line);
  int genes_qty = 0;

  iss >> diseases[i].code;
  iss >> genes_qty;

  diseases[i].genes = new string[genes_qty];

  for (int g = 0; g < genes_qty; g++) {
    iss >> diseases[i].genes[g];
  }

  int disease_chance =
      calculate_disease_chance(dna->dna_sequence, diseases[i].genes,
                               output_string, dna->sub_string_size, genes_qty);
  diseases[i].disease_chance = disease_chance;
  return EXIT_SUCCESS;
}

int read_file(ifstream &input, string &output_string, DNA *&dna) {
  string line;

  // Reading the DNA string data:
  getline(input, line);
  dna->sub_string_size = stoi(line);
  getline(input, dna->dna_sequence);

  // Reading the diseases quantity:
  int diseases_qty;
  getline(input, line);
  diseases_qty = stoi(line);
  Disease *diseases = new Disease[diseases_qty];

  // Allocate a dynamic array to store all disease lines
  string *disease_lines = new string[diseases_qty];
  for (int i = 0; i < diseases_qty; i++) {
    getline(input, disease_lines[i]);
  }

  // Determine the number of available threads
  unsigned int num_threads = thread::hardware_concurrency();
  cout << "Threads number: " << num_threads << endl;
  if (num_threads == 0)
    num_threads = 2; // Fallback in case detection fails

  // Calculate the size of each chunk (balancing the lines)
  int chunk_size = diseases_qty / num_threads;
  int remainder = diseases_qty % num_threads;

  // Create a dynamic array of threads
  thread *threads = new thread[num_threads];

  int start_index = 0;
  for (unsigned int t = 0; t < num_threads; t++) {
    int start = start_index;
    int end = start + chunk_size + (t < (unsigned int)remainder ? 1 : 0);
    threads[t] =
        thread([start, end, disease_lines, &output_string, dna, diseases]() {
          for (int i = start; i < end; i++) {
            process_disease(output_string, disease_lines[i], dna, diseases, i);
          }
        });
    start_index = end;
  }

  // Wait for all threads to finish
  for (unsigned int t = 0; t < num_threads; t++) {
    threads[t].join();
  }

  sortDiseases(diseases, diseases_qty);

  for (int i = 0; i < diseases_qty; i++) {
    output_string +=
        diseases[i].code + "->" + to_string(diseases[i].disease_chance) + "%\n";
  }

  delete[] diseases;

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

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

class LabyrinthNode {
public:
  int cln;
  int row;
  bool isFree;
};

class LabyrinthList {
public:
  LabyrinthNode *list = nullptr;
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

int read_file(ifstream &input, string &output_string,
              LabyrinthList *&labyrinth_list) {

  string line = "";

  getline(input, line);
  int labyrinths_num = stoi(line);
  labyrinth_list->list = new LabyrinthNode[labyrinths_num];

  for (int i = 0; i < labyrinths_num; i++) {
  }

  return EXIT_SUCCESS;
}

int main(int args, char *argv[]) {
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

  LabyrinthList *labyrinth_list = new LabyrinthList();
  read_file(input, output_string, labyrinth_list);

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "Execution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

enum Freedom { FREE, NOT_FREE, START };

enum Directions { RIGHT, TOP, LEFT, BOTTOM };

class LabyrinthNode {
public:
  int row = 0;
  int cln = 0;
  Freedom freedom;
};

class Labyrinth {
public:
  int rows = 0;
  int columns = 0;
  int *start_pos;
  bool has_exit;
  LabyrinthNode **grid = nullptr;
};

class LabyrinthList {
public:
  int size = 0;
  Labyrinth *list = nullptr;
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
  labyrinth_list->size = labyrinths_num;
  labyrinth_list->list = new Labyrinth[labyrinths_num];

  // Reading each labyrinth:
  for (int l = 0; l < labyrinths_num; l++) {
    getline(input, line);
    istringstream iss(line);
    iss >> labyrinth_list->list[l].columns;
    iss >> labyrinth_list->list[l].rows;

    const int rows = labyrinth_list->list[l].rows;
    const int cols = labyrinth_list->list[l].columns;

    labyrinth_list->list[l].grid = new LabyrinthNode *[rows];
    labyrinth_list->list[l].start_pos = new int[2];

    // Reading each labyrinth grid positions;
    for (int i = 0; i < labyrinth_list->list[l].rows; i++) {
      labyrinth_list->list[l].grid[i] = new LabyrinthNode[cols];

      getline(input, line);
      istringstream iss(line);
      string curr_freedom = "";
      for (int j = 0; iss >> curr_freedom; j++) {
        switch (curr_freedom.at(0)) {
        case '0':
          labyrinth_list->list[l].grid[i][j].freedom = FREE;
          break;
        case '1':
          labyrinth_list->list[l].grid[i][j].freedom = NOT_FREE;
          break;
        default:
          labyrinth_list->list[l].grid[i][j].freedom = START;
          labyrinth_list->list[l].start_pos[0] = i;
          labyrinth_list->list[l].start_pos[1] = j;
          break;
        }
      }
    }
  }
  return EXIT_SUCCESS;
}

int does_it_have_exit(Labyrinth &labyrinth) {
  int rows_num = labyrinth.rows;
  int cols_num = labyrinth.columns;

  for (int i = 0, j = 0; i < rows_num; i++)
    if (labyrinth.grid[i][j].freedom == FREE)
      labyrinth.has_exit = true;

  for (int j = 0, i = 0; j < cols_num; j++)
    if (labyrinth.grid[i][j].freedom == FREE)
      labyrinth.has_exit = true;

  for (int j = 0, i = rows_num - 1; j < cols_num; j++)
    if (labyrinth.grid[i][j].freedom == FREE)
      labyrinth.has_exit = true;

  for (int i = 0, j = cols_num - 1; i < rows_num; i++)
    if (labyrinth.grid[i][j].freedom == FREE)
      labyrinth.has_exit = true;

  return EXIT_SUCCESS;
}

int find_lab_exit(Labyrinth &labyrinth) {}

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

  for (int l = 0; l < labyrinth_list->size; l++) {
    does_it_have_exit(labyrinth_list->list[l]);
    cout << labyrinth_list->list[l].has_exit << endl;
  }

  // for (int l = 0; l < labyrinth_list->size; l++) {
  //   for (int i = 0; i < labyrinth_list->list[l].rows; i++) {
  //     for (int j = 0; j < labyrinth_list->list[l].columns; j++) {
  //       cout << labyrinth_list->list[l].grid[i][j].freedom << " ";
  //     }
  //     cout << endl;
  //   }
  //   cout << endl;
  // }

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "Execution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}

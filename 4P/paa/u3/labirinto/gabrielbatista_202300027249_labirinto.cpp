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

class Positions {
public:
  int positions[2];
  struct PositionType *next_pos = nullptr;
  struct PositionType *prev_pos = nullptr;
};

class PositionsList {
public:
  Positions *root = nullptr;
  int size;
};
void add_position(PositionsList *list, int row, int col) {
  // Cria um novo nó
  Positions *newNode = new Positions();
  newNode->positions[0] = row;
  newNode->positions[1] = col;
  newNode->next_pos = nullptr;
  newNode->prev_pos = nullptr;

  // Se a lista estiver vazia, o novo nó será a raiz
  if (list->root == nullptr) {
    list->root = newNode;
  } else {
    // Encontra o último nó da lista
    Positions *current = list->root;
    while (current->next_pos != nullptr) {
      current = current->next_pos;
    }
    // Adiciona o novo nó no final da lista
    current->next_pos = newNode;
    newNode->prev_pos = current;
  }

  // Incrementa o tamanho da lista
  list->size++;
}

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

int find_lab_exit(Labyrinth &labyrinth, string &output_string) {
  cout << "current path: " << endl;

  Directions curr_dir = RIGHT;
  int prev_pos[2] = {};
  int curr_pos[2] = {labyrinth.start_pos[0], labyrinth.start_pos[1]};
  PositionsList *positions_list = new PositionsList();
  Positions *root = new Positions;
  positions_list->root = root;
  positions_list->size = 1;
  positions_list->root->positions[0] = curr_pos[0];
  positions_list->root->positions[1] = curr_pos[1];

  while (true) {
    switch (curr_dir) {
    case RIGHT:
      // Verifies if the current position is not on the wall and free:
      if (curr_pos[1] < labyrinth.columns - 1 &&
          labyrinth.grid[curr_pos[0]][curr_pos[1] + 1].freedom == FREE) {
        curr_pos[1]++;
      } else {
        // This condition will be triggered when does not have way out:
        cout << "Não há saída para a direita." << endl;
        curr_dir = TOP;
      }
      break;

    case TOP:
      // Verifies if the current position is not on the wall and free:
      if (curr_pos[0] > 0 &&
          labyrinth.grid[curr_pos[0] - 1][curr_pos[1]].freedom == FREE) {
        prev_pos[0] = curr_pos[0];
        prev_pos[1] = curr_pos[1];
        curr_pos[0]--;
      } else {
        // This condition will be triggered when does not have way out:
        cout << "Não há saída para a cima." << endl;
        curr_dir = LEFT;
      }
      break;

    case LEFT:
      // Verifies if the current position is not on the wall and free:
      if (curr_pos[1] > 0 &&
          labyrinth.grid[curr_pos[0]][curr_pos[1] - 1].freedom == FREE) {
        curr_pos[1]--;
      } else {
        // This condition will be triggered when does not have way out:
        cout << "Não há saída para a esquerda." << endl;
        curr_dir = BOTTOM;
      }
      break;

    case BOTTOM:
      // Verifies if the current position is not on the wall and free:
      if (curr_pos[0] < labyrinth.rows - 1 &&
          labyrinth.grid[curr_pos[0] + 1][curr_pos[1]].freedom == FREE) {
        curr_pos[0]++;
      } else {
        // This condition will be triggered when does not have way out:
        cout << "Não há saída para baixo." << endl;
        labyrinth.grid[curr_pos[0]][curr_pos[1]].freedom = NOT_FREE;

        curr_dir = RIGHT;
      }
      break;

    default:
      break;
    }
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

  for (int l = 1; l < labyrinth_list->size; l++) {
    does_it_have_exit(labyrinth_list->list[l]);
    find_lab_exit(labyrinth_list->list[l], output_string);
  }

  for (int l = 0; l < labyrinth_list->size; l++) {
    for (int i = 0; i < labyrinth_list->list[l].rows; i++) {
      for (int j = 0; j < labyrinth_list->list[l].columns; j++) {
        cout << labyrinth_list->list[l].grid[i][j].freedom << " ";
      }
      cout << endl;
    }
    cout << endl;
  }

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;

  double ram_after = getMemoryUsageMB();

  cout << "Execution time: " << duration.count() << " s" << endl;
  cout << "Memory Usage: " << (ram_after - ram_before) << " MB" << endl;

  return EXIT_SUCCESS;
}

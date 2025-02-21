#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
using namespace std;

int main(int argc, char *argv[]) {
  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "erro ao abrir input" << endl;
    return EXIT_FAILURE;
  }

  cout << "Input aberto com sucesso!" << endl;

  if (!output.is_open()) {
    cerr << "erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }

  output << "Hello World!" << endl;
  return EXIT_SUCCESS;
}

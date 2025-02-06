#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

class Vehicle {
public:
  string sign;
  int weight;
  int volume;
};

class Package {
public:
  string code;
  float value;
  float weight;
  float volume;
};

int main(int args, char *argv[]) {

  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "Error on input file opening" << endl;
    return EXIT_FAILURE;
  }

  if (!output.is_open()) {
    cerr << "Error on output file opening" << endl;
    return EXIT_FAILURE;
  }

  cout << "Hello World!" << endl;
  return EXIT_SUCCESS;
}

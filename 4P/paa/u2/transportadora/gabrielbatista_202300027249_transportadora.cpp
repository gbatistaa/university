#include <cstdlib>
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

  cout << "Hello World!" << endl;
  return EXIT_SUCCESS;
}

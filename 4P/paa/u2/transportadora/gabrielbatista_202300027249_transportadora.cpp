#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class Vehicle {
public:
  string sign;
  int weight = 0;
  int volume = 0;
};

class VehicleList {
public:
  Vehicle *list = nullptr;
  int size = 0;
};

class Package {
public:
  string code;
  float value = 0;
  float weight = 0;
  float volume = 0;
};

class PackageList {
public:
  Package *list = nullptr;
  int size = 0;
};

int read_file(ifstream &input, VehicleList *&vehicle_list,
              PackageList *&package_list) {

  // Reading and allocating memory for the vehicle list:
  string line;
  getline(input, line);
  istringstream iss(line);
  iss >> vehicle_list->size;
  vehicle_list->list = new Vehicle[vehicle_list->size];

  // Reading all the vehicles data:
  int v = 0;
  while (true) {
    getline(input, line);
    istringstream iss(line);
    if (line.length() < 5) {
      break;
    }
    // Put the line data on the vehicles list (tested):
    iss >> vehicle_list->list[v].sign;
    iss >> vehicle_list->list[v].weight;
    iss >> vehicle_list->list[v++].volume;
  }
  // Reading and allocating memory for the package list:

  package_list->size = stoi(line);
  package_list->list = new Package[package_list->size];

  // Reading all the packages data:
  int p = 0;
  while (getline(input, line)) {
    istringstream iss(line);
    iss >> package_list->list[p].code;
    iss >> package_list->list[p].value;
    iss >> package_list->list[p].weight;
    iss >> package_list->list[p++].volume;
  }

  return EXIT_SUCCESS;
}

int main(int args, char *argv[]) {

  ifstream input(argv[1]);
  ofstream output(argv[2]);

  if (!input.is_open()) {
    cerr << "erro ao abrir input" << endl;
    return EXIT_FAILURE;
  }

  cout << "Input aberto com sucesso!\n" << endl;

  if (!output.is_open()) {
    cerr << "erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }

  cout << "Output aberto com sucesso!\n" << endl;

  PackageList *package_list = new PackageList();
  VehicleList *vehicle_list = new VehicleList();

  read_file(input, vehicle_list, package_list);

  return EXIT_SUCCESS;
}

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace std::chrono;

class Vehicle {
public:
  string sign;
  int max_weight = 0;
  int max_volume = 0;
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
  int weight = 0;
  int volume = 0;
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
    iss >> vehicle_list->list[v].max_weight;
    iss >> vehicle_list->list[v++].max_volume;
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

int maximize_vehicle_value(PackageList *package_list, Vehicle vehicle,
                           string &output_string) {
  const int packages = package_list->size + 1;
  const int max_weight = vehicle.max_weight + 1;
  const int max_volume = vehicle.max_volume + 1;

  // Initializing the backpack with null values:
  float **backpack = new float *[max_weight];
  for (int w = 0; w < max_weight; w++) {
    backpack[w] = new float[max_volume]{0};
  }

  for (int i = 1; i < packages; i++) {
    for (int w = 0; w < max_weight; w++) {
      for (int v = 0; v < max_volume; v++) {
        if (package_list->list[i - 1].weight <= w &&
            package_list->list[i - 1].volume <= v) {
          backpack[w][v] =
              backpack[w][v] < backpack[w - package_list->list[i - 1].weight]
                                       [v - package_list->list[i - 1].volume] +
                                   package_list->list[i - 1].value
                  ? backpack[w][v]
                  : backpack[w - package_list->list[i - 1].weight]
                            [v - package_list->list[i - 1].volume] +
                        package_list->list[i - 1].value;
        }
      }
    }
  }
  return EXIT_SUCCESS;
}

int main(int args, char *argv[]) {
  auto start = high_resolution_clock::now();

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

  cout << "Output aberto com sucesso!\n" << endl;

  PackageList *package_list = new PackageList();
  VehicleList *vehicle_list = new VehicleList();
  string output_string;

  read_file(input, vehicle_list, package_list);

  for (int i = 0; i < vehicle_list->size; i++) {
    maximize_vehicle_value(package_list, vehicle_list->list[i], output_string);
  }

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;
  cout << "Execution time: " << duration.count() << " s" << endl;

  return EXIT_SUCCESS;
}

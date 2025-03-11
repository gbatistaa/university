#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

#define STORED -1

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

int maximize_vehicle_value(PackageList *&package_list, Vehicle vehicle,
                           string &output_string) {
  int packages = package_list->size + 1;
  int max_weight = vehicle.max_weight + 1;
  int max_volume = vehicle.max_volume + 1;

  // Initializing the backpack with null values:
  float **backpack[packages];
  for (int i = 0; i < packages; i++) {
    backpack[i] = new float *[max_weight];
    for (int w = 0; w < max_weight; w++) {
      backpack[i][w] = new float[max_volume];
      for (int v = 0; v < max_volume; v++) {
        backpack[i][w][v] = 0.0f;
      }
    }
  }

  for (int i = 1; i < packages; i++) {
    for (int w = 1; w < max_weight; w++) {
      for (int v = 1; v < max_volume; v++) {
        if (package_list->list[i - 1].weight <= w &&
            package_list->list[i - 1].volume <= v) {
          float sum = backpack[i - 1][w - package_list->list[i - 1].weight]
                              [v - package_list->list[i - 1].volume] +
                      package_list->list[i - 1].value;
          float prev = backpack[i - 1][w][v];
          backpack[i][w][v] = sum > prev ? sum : prev;
        } else {
          backpack[i][w][v] = backpack[i - 1][w][v];
        }
      }
    }
  }

  float max_value = backpack[packages - 1][max_weight - 1][max_volume - 1];

  ostringstream oss;
  oss << fixed << setprecision(2) << max_value;

  output_string += "[" + vehicle.sign + "]R$" + oss.str() + ",";
  oss.str("");

  string used_pkgs_codes = "";
  int a = packages - 1, b = max_weight - 1, c = max_volume - 1;
  float used_weight = 0, used_volume = 0;
  while (a > 0 && b > 0 && c > 0) {
    if (backpack[a][b][c] != backpack[a - 1][b][c]) {
      b -= package_list->list[a - 1].weight;
      c -= package_list->list[a - 1].volume;
      used_weight += package_list->list[a - 1].weight;
      used_volume += package_list->list[a - 1].volume;
      used_pkgs_codes = package_list->list[a - 1].code + "," + used_pkgs_codes;
      package_list->list[a - 1].value = STORED;
    }
    a--;
  }
  used_pkgs_codes.pop_back();
  float uw_perct = ceil((used_weight / vehicle.max_weight) * 100);
  float uv_perct = ceil((used_volume / vehicle.max_volume) * 100);

  oss << fixed << setprecision(0) << used_weight;
  output_string += oss.str() + "KG";
  oss.str("");

  oss << fixed << setprecision(0) << uw_perct;
  output_string += "(" + oss.str() + "%),";
  oss.str("");

  oss << fixed << setprecision(0) << used_volume;
  output_string += oss.str() + "L";
  oss.str("");

  oss << fixed << setprecision(0) << uv_perct;
  output_string += "(" + oss.str() + "%)->" + used_pkgs_codes;
  oss.str("");

  return EXIT_SUCCESS;
}

int calculate_pendencies(PackageList *&package_list, string &output_string) {
  float pendent_value = 0, pendent_weight = 0, pendent_volume = 0;
  string pendent_packages_codes = "";

  for (int i = package_list->size - 1; i >= 0; i--) {
    if (package_list->list[i].value != STORED) {
      pendent_value += package_list->list[i].value;
      pendent_weight += package_list->list[i].weight;
      pendent_volume += package_list->list[i].volume;
      pendent_packages_codes =
          package_list->list[i].code + "," + pendent_packages_codes;
    }
  }
  pendent_packages_codes.pop_back();
  ostringstream oss;

  oss << fixed << setprecision(2) << pendent_value;
  output_string += "[PENDENTE]:R$" + oss.str() + ",";
  oss.str("");
  oss << fixed << setprecision(0) << pendent_weight;
  output_string += oss.str() + "KG,";
  oss.str("");
  oss << fixed << setprecision(0) << pendent_volume;
  output_string += oss.str() + "L->" + pendent_packages_codes;

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
    output_string += "\n";
  }

  calculate_pendencies(package_list, output_string);

  output << output_string;

  auto end = high_resolution_clock::now();
  duration<double> duration = end - start;
  cout << "Execution time: " << duration.count() << " s" << endl;

  return EXIT_SUCCESS;
}

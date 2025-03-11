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
  string line;
  getline(input, line);
  istringstream iss(line);
  iss >> vehicle_list->size;
  vehicle_list->list = new Vehicle[vehicle_list->size];
  int v = 0;
  while (true) {
    getline(input, line);
    istringstream iss(line);
    if (line.length() < 5)
      break;
    iss >> vehicle_list->list[v].sign >> vehicle_list->list[v].max_weight >>
        vehicle_list->list[v++].max_volume;
  }
  package_list->size = stoi(line);
  package_list->list = new Package[package_list->size];
  int p = 0;
  while (getline(input, line)) {
    istringstream iss(line);
    iss >> package_list->list[p].code >> package_list->list[p].value >>
        package_list->list[p].weight >> package_list->list[p++].volume;
  }
  return EXIT_SUCCESS;
}

int maximize_vehicle_value(PackageList *&package_list, Vehicle vehicle,
                           string &output_string) {
  int n = package_list->size;
  int W = vehicle.max_weight + 1;
  int V = vehicle.max_volume + 1;

  // Single DP array
  float **dp = new float *[W];
  for (int w = 0; w < W; w++) {
    dp[w] = new float[V](); // Zero-initialized
  }

  // Decision array remains 3D
  int ***decision = new int **[n + 1];
  for (int i = 0; i <= n; i++) {
    decision[i] = new int *[W];
    for (int w = 0; w < W; w++) {
      decision[i][w] = new int[V]();
      for (int v = 0; v < V; v++)
        decision[i][w][v] = -1;
    }
  }

  // DP computation with single array
  for (int i = 1; i <= n; i++) {
    Package pkg = package_list->list[i - 1];
    for (int w = W - 1; w >= 0; w--) {
      for (int v = V - 1; v >= 0; v--) {
        if (pkg.value != STORED && w >= pkg.weight && v >= pkg.volume) {
          float candidate =
              dp[w - (int)pkg.weight][v - (int)pkg.volume] + pkg.value;
          if (candidate > dp[w][v]) {
            dp[w][v] = candidate;
            decision[i][w][v] = i - 1;
          }
        }
      }
    }
  }

  float max_value = dp[W - 1][V - 1];
  ostringstream oss;
  oss << fixed << setprecision(2) << max_value;
  output_string += "[" + vehicle.sign + "]R$" + oss.str() + ",";
  oss.str("");

  // Backtracking
  int w = W - 1, v = V - 1, i = n;
  float used_weight = 0, used_volume = 0;
  string used_pkgs_codes;
  while (i > 0 && w >= 0 && v >= 0) {
    if (decision[i][w][v] != -1) {
      int pkgIndex = decision[i][w][v];
      Package pkg = package_list->list[pkgIndex];
      used_pkgs_codes = pkg.code + "," + used_pkgs_codes;
      used_weight += pkg.weight;
      used_volume += pkg.volume;
      package_list->list[pkgIndex].value = STORED;
      w -= pkg.weight;
      v -= pkg.volume;
    }
    i--;
  }
  if (!used_pkgs_codes.empty())
    used_pkgs_codes.pop_back();

  float uw_perct = round((used_weight / vehicle.max_weight) * 100);
  float uv_perct = round((used_volume / vehicle.max_volume) * 100);
  oss << fixed << setprecision(0) << used_weight;
  output_string += oss.str() + "KG(";
  oss.str("");
  oss << fixed << setprecision(0) << uw_perct;
  output_string += oss.str() + "%),";
  oss.str("");
  oss << fixed << setprecision(0) << used_volume;
  output_string += oss.str() + "L(";
  oss.str("");
  oss << fixed << setprecision(0) << uv_perct;
  output_string += oss.str() + "%)->" + used_pkgs_codes;

  // Cleanup
  for (int w = 0; w < W; w++)
    delete[] dp[w];
  delete[] dp;
  for (int i = 0; i <= n; i++) {
    for (int w = 0; w < W; w++)
      delete[] decision[i][w];
    delete[] decision[i];
  }
  delete[] decision;

  return EXIT_SUCCESS;
}

int calculate_pendencies(PackageList *&package_list, string &output_string) {
  float pendent_value = 0.0f, pendent_weight = 0.0f, pendent_volume = 0.0f;
  string pendent_packages_codes;
  for (int i = package_list->size - 1; i >= 0; i--) {
    if (package_list->list[i].value != STORED) {
      pendent_value += package_list->list[i].value;
      pendent_weight += package_list->list[i].weight;
      pendent_volume += package_list->list[i].volume;
      pendent_packages_codes =
          package_list->list[i].code + "," + pendent_packages_codes;
    }
  }
  if (!pendent_packages_codes.empty())
    pendent_packages_codes.pop_back();
  ostringstream oss;
  pendent_value -= 0.01; // Maintain original logic
  oss << fixed << setprecision(2) << pendent_value;
  output_string += "PENDENTE:R$" + oss.str() + ",";
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

  // Cleanup
  delete[] vehicle_list->list;
  delete[] package_list->list;
  delete vehicle_list;
  delete package_list;

  return EXIT_SUCCESS;
}

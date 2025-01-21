#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

float calculate_average(vector<float> vec) {
  float total = 0;
  for (float num : vec) {
    total += num;
  }
  return total / vec.size();
}

int calculate_median(int size) {
  int half_plus = (size + 1) / 2;
  return half_plus - 1;
}

float calculate_variance(vector<float> list, float average) {
  int sigma = 0;
  for (float num : list) {
    float aux = (num - average);
    sigma += (aux * aux);
  }
  return (float)sigma / (list.size() - 1);
}

float calculate_std_dev(float variance) {
  float std_dev = sqrt(variance);
  return std_dev;
}

// Function to calculate modes (multimodal)
vector<float> calculateModes(const vector<float> &data) {
  if (data.empty()) {
    throw invalid_argument("The vector is empty, mode cannot be calculated.");
  }

  unordered_map<float, int> frequencies;
  for (float num : data) {
    frequencies[num]++;
  }

  int maxFrequency = 0;
  for (const auto &pair : frequencies) {
    if (pair.second > maxFrequency) {
      maxFrequency = pair.second;
    }
  }

  vector<float> modes;
  for (const auto &pair : frequencies) {
    if (pair.second == maxFrequency) {
      modes.push_back(pair.first);
    }
  }

  return modes;
}

int main() {
  int op, i = 1;
  float curr_data;
  vector<float> data = {};

  cout << "----------Data Analitics----------" << endl;
  do {
    cout << "Digite o dado " << i << ": ";
    cin >> curr_data;
    data.push_back(curr_data);
    cout << "1- Adicionar dados" << endl;
    cout << "0- Calcular estatísticas" << endl;
    cin >> op;
    i++;
  } while (op != 0);
  sort(data.begin(), data.end());

  float average = calculate_average(data);
  float variance = calculate_variance(data, average);
  float std_dev = calculate_std_dev(variance);
  int median = calculate_median(data.size());
  cout << "\n----------Data Analitics----------" << endl;
  cout << "Average: " << average << endl;
  cout << "Variance: " << variance << endl;
  cout << "Standard dev: " << std_dev << endl;
  // cout << "Median" << data.at(median - 1) << endl;
  try {
    vector<float> modes = calculateModes(data);
    cout << "The mode(s): ";
    for (float mode : modes) {
      cout << mode << " ";
    }
    cout << endl;
  } catch (const invalid_argument &e) {
    cout << "Error: " << e.what() << endl;
  }

  return 0;

  return EXIT_SUCCESS;
}

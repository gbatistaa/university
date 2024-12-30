#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream> // This is the library to manipulate files
#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct container {
  string code;
  string cnpj;
  int weight;
} Container;

void merge(vector<int> &vector, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  int leftSubVector[n1], rightSubVector[n2];

  for (int i = 0; i < n1; i++)
    leftSubVector[i] = vector[left + i];
  for (int j = 0; j < n2; j++)
    rightSubVector[j] = vector[mid + 1 + j];

  int i = 0;
  int j = 0;
  int k = left;

  while (i < n1 && j < n2) {
    if (leftSubVector[i] <= rightSubVector[j]) {
      vector[k] = leftSubVector[i];
      i++;
    } else {
      vector[k] = rightSubVector[j];
      j++;
    }
    k++;
  }
  while (i < n1) {
    vector[k] = leftSubVector[i];
    i++;
    k++;
  }

  while (j < n2) {
    vector[k] = rightSubVector[j];
    j++;
    k++;
  }
}

void mergeSort(vector<int> *vector, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    mergeSort(vector, left, mid);
    mergeSort(vector, mid + 1, right);

    merge(*vector, left, mid, right);
  }
}

int writeContainerProp(string *props[], char character, int &propSts) {
  if (character == ' ') {
    propSts++;
  } else {
    int i = propSts;
    *props[i] += character;
  }
  return EXIT_SUCCESS;
}

vector<Container> readInputAndCreateContainerList(fstream &file) {
  vector<Container> containers = {};
  string fileLine;

  while (getline(file, fileLine)) {
    int lineSize = fileLine.length();

    if (lineSize <= 3) {
      continue;
    }

    Container newContainer;
    string newCnpj, newCode, newWeight;
    int newWeightNumeric, propStatus = 0;
    string *props[3] = {&newCnpj, &newCode, &newWeight};

    for (int i = 0; i < lineSize; i++) {
      writeContainerProp(props, fileLine.at(i), propStatus);
    }
    newWeightNumeric = stoi(newWeight);

    newContainer.cnpj = newCnpj;
    newContainer.code = newCode;
    newContainer.weight = newWeightNumeric;

    containers.push_back(newContainer);
  }
  return containers;
}

int main() {
  fstream input("poxim-tech.txt");
  if (!input.is_open()) {
    cout << "Error on file Opening.";
    return EXIT_FAILURE;
  }

  vector<Container> containers = readInputAndCreateContainerList(input);

  for (int i = 0; i < containers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "CNPJ: " << containers.at(i).cnpj << "\n";
    cout << "Code: " << containers.at(i).code << "\n";
    cout << "Weight: " << containers.at(i).weight << " kg\n\n";
  }

  return EXIT_SUCCESS;
}

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

int readInputAndCreateContainerLists(fstream &file,
                                     vector<Container> *contPointers[]) {
  vector<Container> containers = {};
  string fileLine;
  int currContainerIndex = 0;

  // Ignoring the first line of the file
  getline(file, fileLine);

  while (getline(file, fileLine)) {
    int lineSize = fileLine.length();

    // Verification to create the next container list
    if (lineSize <= 3) {
      currContainerIndex++;
      continue;
    }

    Container newContainer;
    string newCnpj, newCode, newWeight;
    int newWeightNumeric, propStatus = 0;
    string *props[3] = {&newCnpj, &newCode, &newWeight};

    // Reading the file, separating in props and writing in the string variables
    for (int i = 0; i < lineSize; i++) {
      writeContainerProp(props, fileLine.at(i), propStatus);
    }
    newWeightNumeric = stoi(newWeight);

    // Writing the colected data in the props of the new container
    newContainer.cnpj = newCnpj;
    newContainer.code = newCode;
    newContainer.weight = newWeightNumeric;

    containers.push_back(newContainer);
    contPointers[currContainerIndex]->push_back(newContainer);
  }

  return EXIT_SUCCESS;
}

int main() {
  fstream input("poxim-tech.txt");
  if (!input.is_open()) {
    cout << "Error on file Opening.";
    return EXIT_FAILURE;
  }

  vector<Container> containers;
  vector<Container> fiscalizedContainers;

  vector<Container> *containersPointers[2] = {&containers,
                                              &fiscalizedContainers};

  readInputAndCreateContainerLists(input, containersPointers);

  cout << "----------Registered Containers----------\n\n";
  for (int i = 0; i < containers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "CNPJ: " << containers.at(i).cnpj << "\n";
    cout << "Code: " << containers.at(i).code << "\n";
    cout << "Weight: " << containers.at(i).weight << " kg\n\n";
  }

  cout << "----------Fiscalized Containers----------\n\n";
  for (int i = 0; i < fiscalizedContainers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "CNPJ: " << fiscalizedContainers.at(i).cnpj << "\n";
    cout << "Code: " << fiscalizedContainers.at(i).code << "\n";
    cout << "Weight: " << fiscalizedContainers.at(i).weight << " kg\n\n";
  }

  return EXIT_SUCCESS;
}

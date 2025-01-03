#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream> 
#include <iostream>
#include <string>
#include <vector>

#define FAIL -1;

using namespace std;

typedef struct container {
  string code;
  string cnpj;
  int weight;
} Container;

vector<Container> filterRegisteredContainersForInspection(
    vector<Container> registeredContainers,
    vector<Container> containersToInspection) {

  vector<Container> duplicatedContainers = {};

  for (Container registered : registeredContainers) {
    for (Container toInspec : containersToInspection) {

      // Verification to see if the registered container is gonna be
      // inspectioned
      if (registered.code == toInspec.code) {
        duplicatedContainers.push_back(registered);
      }
    }
  }
  return duplicatedContainers;
}

bool hasDifferentCnjp(Container container, vector<Container> inspectioneds,
                      Container *cont = NULL) {
  for (Container inspectioned : inspectioneds) {
    if (container.cnpj == inspectioned.cnpj) {
      cont = &container;
      return true;
    }
  }
  return false;
}

bool areFloatsEqual(float a, float b, float e = 1e-6) {
  return fabs(a - b) < e;
}

bool isLessThan(float a, float b, float e = 1e-6) {
  return (a < b) && fabs(a - b) > e;
}

bool isGreaterThan(float a, float b, float e = 1e-6) {
  return (a > b) && fabs(a - b) > e;
}

float calculateDifPercent(int n1, int n2) {
  int dif = n1 - n2;
  float percentage = (float)dif / n1 * 100;
  return abs(percentage);
}

float calcContainerWeightDifPercent(Container container,
                                    vector<Container> inspectioneds,
                                    Container *pointer = nullptr) {
  for (Container inspectioned : inspectioneds) {
    if (container.code == inspectioned.code) {
      float difPercent =
          calculateDifPercent(container.weight, inspectioned.weight);
      if (isLessThan(difPercent, 10))
        pointer = &container;
      return difPercent;
    }
  }
  return FAIL;
}

void mergeContainers(vector<Container> &registereds,
                     vector<Container> inspectioneds, int left, int mid,
                     int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  Container leftSubVector[n1], rightSubVector[n2];

  for (int i = 0; i < n1; i++)
    leftSubVector[i] = registereds[left + i];
  for (int j = 0; j < n2; j++)
    rightSubVector[j] = registereds[mid + 1 + j];

  int i = 0;
  int j = 0;
  int k = left;
  Container containerLeft, containerRight;

  // Sorting the containers by CNPJ and weight diference:
  while (i < n1 && j < n2) {
    containerLeft = leftSubVector[i];
    containerRight = rightSubVector[j];
    Container *containerPtr = nullptr;

    // Case 1: Both containers has different CNPJ
    if (hasDifferentCnjp(containerLeft, inspectioneds) &&
        hasDifferentCnjp(containerRight, inspectioneds)) {
      registereds[k] = containerLeft;
      i++;
    }

    // Case 2: Only one of the containers has different CNPJ
    else if (hasDifferentCnjp(containerLeft, inspectioneds, containerPtr) ||
             hasDifferentCnjp(containerRight, inspectioneds, containerPtr)) {
      registereds[k] = *containerPtr;
      if (containerPtr == &containerLeft)
        i++;
      else
        j++;
    }

    // Case 3: None of the containers has different registered CNPJ
    else {
      Container *contPointer = nullptr;
      float difContL = calcContainerWeightDifPercent(
          containerLeft, inspectioneds, contPointer);
      float difContR = calcContainerWeightDifPercent(
          containerRight, inspectioneds, contPointer);

      // Case 3.1: Both containers has more than 10% of inspectioned difference
      if (contPointer == nullptr) {
        // Case 3.1.1: Both containers are available for inspection and are
        // equals
        if (areFloatsEqual(difContL, difContR)) {
          registereds[k] = containerLeft;
          i++;

          // Equals but right container weight is bigger
        } else if (isGreaterThan(difContL, difContR)) {
        }
        // Equals but right container weight is bigger
        else if (isLessThan(difContL, difContR)) {
        }
      }
    }
    k++;
  }

  // Wrtiting the remaining containers on the vector:
  while (i < n1) {
    containerLeft = leftSubVector[i];

    registereds[k] = containerLeft;
    i++;
    k++;
  }
  while (j < n2) {
    containerRight = rightSubVector[j];

    registereds[k] = containerRight;
    j++;
    k++;
  }
}

// If this algorithym works pass it to a estableized version:
int sortContainersForInspection(vector<Container> &registereds,
                                vector<Container> inspectioneds, int left,
                                int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    sortContainersForInspection(registereds, inspectioneds, left, mid);
    sortContainersForInspection(registereds, inspectioneds, mid + 1, right);

    mergeContainers(registereds, inspectioneds, left, mid, right);
  }
  return EXIT_SUCCESS;
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

  vector<Container> registeredContainers;
  vector<Container> fiscalizedContainers;

  vector<Container> *containersPointers[2] = {&registeredContainers,
                                              &fiscalizedContainers};

  readInputAndCreateContainerLists(input, containersPointers);

  vector<Container> duplicatedContainers =
      filterRegisteredContainersForInspection(registeredContainers,
                                              fiscalizedContainers);

  cout << "----------Registered Containers----------\n\n";
  for (int i = 0; i < registeredContainers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "CNPJ: " << registeredContainers.at(i).cnpj << "\n";
    cout << "Code: " << registeredContainers.at(i).code << "\n";
    cout << "Weight: " << registeredContainers.at(i).weight << " kg\n\n";
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

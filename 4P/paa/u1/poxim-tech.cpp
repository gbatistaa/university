#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#define FAIL -1

using namespace std;

enum Irregularity {
  CNPJ,
  WEIGHT,
};

typedef struct container {
  string code;
  string cnpj;
  int weight;
} Container;

typedef struct irregular {
  string code;
  string cnpj;
  int weight;
  Irregularity irregularity;
  string irregularityMessage;
} Irregular;

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
                      string &msg) {
  for (Container inspectioned : inspectioneds) {
    if (container.code == inspectioned.code &&
        container.cnpj != inspectioned.cnpj) {
      msg = container.cnpj + "<->" + inspectioned.cnpj;
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
                                    vector<Container> inspectioneds) {
  for (Container inspectioned : inspectioneds) {
    if (container.code == inspectioned.code) {
      float difPercent =
          calculateDifPercent(container.weight, inspectioned.weight);
      return difPercent;
    }
  }
  return EXIT_FAILURE;
}

vector<Irregular> addIrregularContainer(vector<Container> fiscalizeds,
                                        vector<Container> duplicateds) {
  vector<Irregular> irregulars = {};
  string irrMsg;

  for (Container duplicated : duplicateds) {

    bool difCnpj = hasDifferentCnjp(duplicated, fiscalizeds, irrMsg);
    float weightDif = calcContainerWeightDifPercent(duplicated, fiscalizeds);

    if (difCnpj) {
      Irregular newIrregular;
      newIrregular.code = duplicated.code;
      newIrregular.cnpj = duplicated.cnpj;
      newIrregular.weight = duplicated.weight;
      newIrregular.irregularity = CNPJ;
      newIrregular.irregularityMessage = irrMsg;
      irregulars.push_back(newIrregular);
    }

    else if (isGreaterThan(weightDif, 10)) {
      ostringstream oss;
      oss << fixed << setprecision(1) << weightDif;

      Irregular newIrregular;
      newIrregular.code = duplicated.code;
      newIrregular.cnpj = duplicated.cnpj;
      newIrregular.weight = duplicated.weight;
      newIrregular.irregularity = WEIGHT;
      newIrregular.irregularityMessage = "(" + oss.str() + "%" + ")";
      irregulars.push_back(newIrregular);
    }
  }

  return irregulars;
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
    string *props[3] = {&newCode, &newCnpj, &newWeight};

    // Reading the file, separating in props and writing in the string
    // variables
    for (int i = 0; i < lineSize; i++) {
      writeContainerProp(props, fileLine.at(i), propStatus);
    }
    newWeightNumeric = stoi(newWeight);

    // Writing the colected data in the props of the new container
    newContainer.code = newCode;
    newContainer.cnpj = newCnpj;
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

  cout << "----------Registered Containers----------\n\n";
  for (int i = 0; i < registeredContainers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "Code: " << registeredContainers.at(i).code << "\n";
    cout << "CNPJ: " << registeredContainers.at(i).cnpj << "\n";
    cout << "Weight: " << registeredContainers.at(i).weight << " kg\n\n";
  }

  vector<Container> duplicatedContainers =
      filterRegisteredContainersForInspection(registeredContainers,
                                              fiscalizedContainers);
  vector<Irregular> irregulars =
      addIrregularContainer(fiscalizedContainers, duplicatedContainers);
  // sortContainersForInspection(duplicatedContainers, fiscalizedContainers,
  //                             irregulars, 0, duplicatedContainers.size() -
  //                             1);

  cout << "----------Duplicated Containers----------\n\n";
  for (int i = 0; i < duplicatedContainers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "Code: " << duplicatedContainers.at(i).code << "\n";
    cout << "CNPJ: " << duplicatedContainers.at(i).cnpj << "\n";
    cout << "Weight: " << duplicatedContainers.at(i).weight << " kg\n\n";
  }
  cout << "----------Fiscalized Containers----------\n\n";
  for (int i = 0; i < fiscalizedContainers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "Code: " << fiscalizedContainers.at(i).code << "\n";
    cout << "CNPJ: " << fiscalizedContainers.at(i).cnpj << "\n";
    cout << "Weight: " << fiscalizedContainers.at(i).weight << " kg\n\n";
  }

  cout << "----------Irregulars Containers----------\n\n";
  for (int i = 0; i < irregulars.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "Code: " << irregulars.at(i).code << "\n";
    cout << "CNPJ: " << irregulars.at(i).cnpj << "\n";
    cout << "Weight: " << irregulars.at(i).weight << " kg\n";
    cout << "Irregularity: " << irregulars.at(i).irregularityMessage << "\n\n";
  }

  return EXIT_SUCCESS;
}

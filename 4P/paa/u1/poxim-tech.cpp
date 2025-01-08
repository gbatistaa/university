#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define FAIL -1

using namespace std;

enum Irregularity {
  WEIGHT,
  CNPJ,
};

class Container {
public:
  string code;

public:
  string cnpj;

public:
  int weight;
};

class Irregular : public Container {
public:
  Irregularity irregularity;

public:
  string irregularityMessage;
};

class IrregularWeight : public Irregular {
public:
  float bruteWeightDif;
};

bool areFloatsEqual(float a, float b, float e = 1e-6) {
  return fabs(a - b) < e;
}

bool isGreaterThan(float a, float b, float e = 1e-6) {
  return (a > b) && fabs(a - b) > e;
}

float calculateDifPercent(int n1, int n2) {
  int dif = n1 - n2;
  float percentage = (float)dif / n1 * 100;
  return abs(percentage);
}

vector<Container>
filterRegisteredContainersForInspection(vector<Container> registereds,
                                        map<string, Container> fiscalizeds) {
  vector<Container> duplicateds = {};
  for (Container registered : registereds) {
    // Verifies if the container registered was put to fiscalization
    if (fiscalizeds.find(registered.code) != fiscalizeds.end()) {
      if (registered.code == fiscalizeds.at(registered.code).code)
        duplicateds.push_back(registered);
    }
  }
  return duplicateds;
}

map<string, Container> createContainerMap(vector<Container> containerVector) {
  map<string, Container> newContainerMap;

  for (Container container : containerVector)
    newContainerMap.insert({container.code, container});

  return newContainerMap;
}

bool hasDifferentCnjp(Container container, map<string, Container> fiscalizeds,
                      string &msg) {
  if (fiscalizeds.find(container.code) != fiscalizeds.end()) {
    Container fiscalized = fiscalizeds.at(container.code);
    if (container.code == fiscalized.code &&
        container.cnpj != fiscalized.cnpj) {
      msg = container.cnpj + "<->" + fiscalized.cnpj;
      return true;
    }
  }
  return false;
}

float calcContainerWeightDifPercent(Container container,
                                    map<string, Container> fiscalizeds,
                                    float &bruteWeightDif) {
  try {
    Container fiscalized = fiscalizeds.at(container.code);
    if (container.code == fiscalized.code) {
      float weightDif =
          calculateDifPercent(container.weight, fiscalized.weight);
      bruteWeightDif = fabs(container.weight - fiscalized.weight);
      return weightDif;
    }
    return EXIT_SUCCESS;
  } catch (const exception &e) {
    return EXIT_FAILURE;
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

    // Adding the new container in the containers list:
    contPointers[currContainerIndex]->push_back(newContainer);
  }

  return EXIT_SUCCESS;
}

vector<Irregular> addIrregularContainer(map<string, Container> fiscalizeds,
                                        vector<Container> duplicateds) {
  vector<Irregular> irregulars = {};
  string irrMsg;

  for (Container duplicated : duplicateds) {
    float weightDif;
    bool difCnpj = hasDifferentCnjp(duplicated, fiscalizeds, irrMsg);
    float weightDifPercent =
        calcContainerWeightDifPercent(duplicated, fiscalizeds, weightDif);

    if (difCnpj) {
      Irregular newCnpjIrr;
      newCnpjIrr.code = duplicated.code;
      newCnpjIrr.cnpj = duplicated.cnpj;
      newCnpjIrr.weight = duplicated.weight;
      newCnpjIrr.irregularity = CNPJ;
      newCnpjIrr.irregularityMessage = irrMsg;
      irregulars.push_back(newCnpjIrr);
    }

    else if (isGreaterThan(weightDifPercent, 10)) {
      ostringstream oss;
      oss << fixed << setprecision(0) << weightDifPercent;
      oss << fixed << setprecision(0) << weightDif;
      string irrMsg = to_string(weightDif) + " kg" + " (" + oss.str() + "%)";

      Irregular newWeightIrr;
      newWeightIrr.code = duplicated.code;
      newWeightIrr.cnpj = duplicated.cnpj;
      newWeightIrr.weight = duplicated.weight;
      newWeightIrr.irregularity = WEIGHT;
      newWeightIrr.irregularityMessage = irrMsg;
      irregulars.push_back(newWeightIrr);
    }
  }

  return irregulars;
}

void mergeIrregularContainers(vector<Irregular> &vec,
                              map<string, Container> fiscalizeds, int left,
                              int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  // Vetores temporários
  vector<Irregular> leftVec(n1), rightVec(n2);

  // Copiar os dados para os vetores temporários
  for (int a = 0; a < n1; a++)
    leftVec[a] = vec[left + a];
  for (int b = 0; b < n2; b++)
    rightVec[b] = vec[mid + 1 + b];

  // Mesclar os vetores temporários de volta ao vetor original
  int i = 0, j = 0, k = left;

  while (i < n1 && j < n2) {
    Irregularity leftIrregularity = leftVec.at(i).irregularity;
    Irregularity rightIrregularity = rightVec.at(j).irregularity;

    // Comparsion to prioritize the CNPJ irregularity:
    if (leftIrregularity > rightIrregularity) {
      vec[k++] = leftVec.at(i);
      i++;
    } else if (leftIrregularity == rightIrregularity) {
      if (leftIrregularity == CNPJ) {
        vec[k++] = leftVec.at(i);
        i++;
      } else {
        float weightDifL, weightDifR;

        float leftWeightDif = calcContainerWeightDifPercent(
            leftVec.at(i), fiscalizeds, weightDifL);
        float rightWeightDif = calcContainerWeightDifPercent(
            rightVec.at(j), fiscalizeds, weightDifR);

        if (areFloatsEqual(leftWeightDif, rightWeightDif) ||
            isGreaterThan(leftWeightDif, rightWeightDif)) {
          vec[k++] = leftVec.at(i);
          i++;
        } else {
          vec[k++] = rightVec.at(j);
          j++;
        }
      }
    } else {
      vec[k++] = rightVec.at(j);
      j++;
    }
  }

  while (i < n1) {
    vec[k] = leftVec[i];
    i++;
    k++;
  }

  while (j < n2) {
    vec[k] = rightVec[j];
    j++;
    k++;
  }
}

// Função de ordenação Merge Sort
void sortIrregularContainers(vector<Irregular> &vec,
                             map<string, Container> fiscalizeds, int left,
                             int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    // Ordenar a primeira e a segunda metades
    sortIrregularContainers(vec, fiscalizeds, left, mid);
    sortIrregularContainers(vec, fiscalizeds, mid + 1, right);

    // Mesclar as metades ordenadas
    mergeIrregularContainers(vec, fiscalizeds, left, mid, right);
  }
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

  map<string, Container> fiscalizedsMap =
      createContainerMap(fiscalizedContainers);

  vector<Container> duplicatedContainers =
      filterRegisteredContainersForInspection(registeredContainers,
                                              fiscalizedsMap);
  vector<Irregular> irregulars =
      addIrregularContainer(fiscalizedsMap, duplicatedContainers);

  sortIrregularContainers(irregulars, fiscalizedsMap, 0, irregulars.size() - 1);

  cout << "----------Duplicated Containers----------\n\n";
  for (int i = 0; i < duplicatedContainers.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "Code: " << duplicatedContainers.at(i).code << "\n";
    cout << "CNPJ: " << duplicatedContainers.at(i).cnpj << "\n";
    cout << "Weight: " << duplicatedContainers.at(i).weight << " kg\n\n";
  }

  cout << "----------Irregulars Containers----------\n\n";
  for (int i = 0; i < irregulars.size(); i++) {
    cout << "Container " << i + 1 << ": \n\n";
    cout << "Code: " << irregulars.at(i).code << "\n";
    cout << "CNPJ: " << irregulars.at(i).cnpj << "\n";
    cout << "Weight: " << irregulars.at(i).weight << " kg\n";
    cout << "Irregularity: " << irregulars.at(i).irregularityMessage << "\n\n";
  }

  // mergeSort(irregulars, 0, irregulars.size() - 1);

  return EXIT_SUCCESS;
}

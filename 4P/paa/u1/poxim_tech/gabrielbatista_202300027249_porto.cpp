#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>

using namespace std;

enum Irregularity {
  WEIGHT,
  CNPJ,
};

class Container {
public:
  string code;
  string cnpj;
  int weight;

  Container() : code(""), cnpj(""), weight(0) {}
};

class Irregular : public Container {
public:
  Irregularity irregularity;
  string irregularityMessage;
};

typedef struct containerList {
  Container *list;
  int size;
} ContainerList;

typedef struct irregularList {
  Irregular *list;
  int size;
} IrregularList;

bool isGreaterThan(float a, float b, float e = 1e-6) {
  return (a > b) && fabs(a - b) > e;
}

float calculateDifPercent(int n1, int n2) {
  int dif = n1 - n2;
  float percentage = (float)dif / n1 * 100;
  return abs(percentage);
}

ContainerList *filterRegisteredContainersForInspection(
    ContainerList *registereds, int registeredsSize,
    unordered_map<string, Container> fiscalizedsMap) {
  ContainerList *duplicateds = new ContainerList[1];
  if (!duplicateds) {
    exit(EXIT_FAILURE);
  }
  duplicateds->list = nullptr;
  duplicateds->size = 0;

  Container *newDuplicatedsList = new Container[1];
  duplicateds->list = newDuplicatedsList;
  for (int i = 0; i < registeredsSize; i++) {
    try {
      // Verifies if the registered container was fiscalized:
      if (registereds->list[i].code ==
          fiscalizedsMap.at(registereds->list[i].code).code) {

        // Increments the duplicated containers list size:
        duplicateds->size++;

        Container *resizedList = new Container[duplicateds->size];
        for (int i = 0; i < duplicateds->size - 1; i++) {
          resizedList[i] = duplicateds->list[i];
        }

        duplicateds->list = resizedList;

        // Add the duplicated container in the list last position:
        duplicateds->list[duplicateds->size - 1] = registereds->list[i];
      }
    } catch (const exception &e) {
      cerr << e.what() << endl;
    }
  }
  return duplicateds;
}

unordered_map<string, Container> createContainerMap(ContainerList *fiscalizeds,
                                                    int fiscSize) {
  unordered_map<string, Container> newContainerMap;
  for (int i = 0; i < fiscSize; i++) {
    newContainerMap[fiscalizeds->list[i].code] = fiscalizeds->list[i];
  }
  return newContainerMap;
}

bool hasDifferentCnjp(Container container,
                      unordered_map<string, Container> fiscalizeds,
                      string &msg) {
  Container fiscalized = fiscalizeds.at(container.code);
  if (container.code == fiscalized.code && container.cnpj != fiscalized.cnpj) {
    msg = container.cnpj + "<->" + fiscalized.cnpj;
    return true;
  }
  return false;
}

float calcContainerWeightDifPercent(
    Container container, unordered_map<string, Container> fiscalizeds,
    float *bruteWeightDif = nullptr) {
  try {
    Container fiscalized = fiscalizeds.at(container.code);
    if (container.code == fiscalized.code) {
      float weightDif =
          calculateDifPercent(container.weight, fiscalized.weight);
      if (bruteWeightDif != nullptr)
        *bruteWeightDif = fabs(container.weight - fiscalized.weight);
      return weightDif;
    }
    return EXIT_SUCCESS;
  } catch (const exception &e) {
    return EXIT_FAILURE;
  }
}

IrregularList *
createIrregularContainersList(unordered_map<string, Container> fiscalizeds,
                              ContainerList *duplicateds, int duplicatedsSize) {
  string irrMsg;
  IrregularList *irregulars = new IrregularList[1];
  irregulars->size = 0;

  Irregular *irrList = new Irregular[1];
  irregulars->list = irrList;

  for (int i = 0; i < duplicatedsSize; i++) {
    Container duplicated = duplicateds->list[i];

    float weightDif;
    bool difCnpj = hasDifferentCnjp(duplicated, fiscalizeds, irrMsg);
    float weightDifPercent =
        calcContainerWeightDifPercent(duplicated, fiscalizeds, &weightDif);

    if (difCnpj) {
      Irregular newCnpjIrr;
      newCnpjIrr.code = duplicated.code;
      newCnpjIrr.cnpj = duplicated.cnpj;
      newCnpjIrr.weight = duplicated.weight;
      newCnpjIrr.irregularity = CNPJ;
      newCnpjIrr.irregularityMessage = duplicated.code + ":" + irrMsg;
      irregulars->size++;

      Irregular *resizedIrrList = new Irregular[irregulars->size];
      for (int i = 0; i < irregulars->size - 1; i++) {
        resizedIrrList[i] = irregulars->list[i];
      }

      irregulars->list = resizedIrrList;
      irregulars->list[irregulars->size - 1] = newCnpjIrr;
    }

    else if (isGreaterThan(weightDifPercent, 10)) {

      ostringstream difPerc, bruteDif;
      difPerc << fixed << setprecision(0) << weightDifPercent;
      bruteDif << fixed << setprecision(0) << weightDif;
      string irrMsg = bruteDif.str() + "kg(" + difPerc.str() + "%)";

      Irregular newWeightIrr;
      newWeightIrr.code = duplicated.code;
      newWeightIrr.cnpj = duplicated.cnpj;
      newWeightIrr.weight = duplicated.weight;
      newWeightIrr.irregularity = WEIGHT;
      newWeightIrr.irregularityMessage = duplicated.code + ":" + irrMsg;
      irregulars->size++;

      Irregular *resizedIrrList = new Irregular[irregulars->size];
      for (int i = 0; i < irregulars->size - 1; i++) {
        resizedIrrList[i] = irregulars->list[i];
      }

      irregulars->list = resizedIrrList;
      irregulars->list[irregulars->size - 1] = newWeightIrr;
    }
  }

  return irregulars;
}

int mergeIrregularContainers(Irregular *irregulars,
                             unordered_map<string, Container> fiscalizeds,
                             int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  // Vetores temporários
  Irregular leftVec[n1], rightVec[n2];

  // Copiar os dados para os vetores temporários
  for (int a = 0; a < n1; a++)
    leftVec[a] = irregulars[left + a];
  for (int b = 0; b < n2; b++)
    rightVec[b] = irregulars[mid + 1 + b];

  // Mesclar os vetores temporários de volta ao vetor original
  int i = 0, j = 0, k = left;

  while (i < n1 && j < n2) {
    Irregularity leftIrregularity = leftVec[i].irregularity;
    Irregularity rightIrregularity = rightVec[j].irregularity;

    // Comparsion to prioritize the CNPJ irregularity:
    if (leftIrregularity > rightIrregularity) {
      irregulars[k++] = leftVec[i++];

      // Comparision to prioritize the container with the bigger weight
      // irregularity:
    } else if (leftIrregularity == rightIrregularity) {
      if (leftIrregularity == WEIGHT) {
        int weightIrrL =
            round(calcContainerWeightDifPercent(leftVec[i], fiscalizeds));
        int weightIrrR =
            round(calcContainerWeightDifPercent(rightVec[j], fiscalizeds));

        if (isGreaterThan(weightIrrL, weightIrrR) || weightIrrL == weightIrrR) {
          irregulars[k++] = leftVec[i++];
        } else {
          irregulars[k++] = rightVec[j++];
        }
      } else {
        irregulars[k++] = leftVec[i++];
      }
      // Comparision to prioritize the container with CNPJ irregularity that
      // comes first on the register list:
    } else {
      irregulars[k++] = rightVec[j++];
    }
  }

  while (i < n1) {
    irregulars[k++] = leftVec[i++];
  }

  while (j < n2) {
    irregulars[k++] = rightVec[j++];
  }
  return EXIT_SUCCESS;
}

// Função de ordenação Merge Sort
void sortIrregularContainers(Irregular *irregulars,
                             unordered_map<string, Container> fiscalizeds,
                             int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    // Ordenar a primeira e a segunda metades
    sortIrregularContainers(irregulars, fiscalizeds, left, mid);
    sortIrregularContainers(irregulars, fiscalizeds, mid + 1, right);

    // Mesclar as metades ordenadas
    mergeIrregularContainers(irregulars, fiscalizeds, left, mid, right);
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

int readInputAndCreateContainerLists(ifstream &file, ContainerList **regis,
                                     ContainerList **fiscs) {
  string fileLine;
  int currIndex = 0;
  int currVectorSize = 0;
  bool isFirstCont = true;

  // Allocate memory for the ContainerList structures
  *regis = new ContainerList();
  (*regis)->size = 0;
  (*regis)->list = nullptr;

  *fiscs = new ContainerList();
  (*fiscs)->size = 0;
  (*fiscs)->list = nullptr;

  ContainerList **currList = nullptr;
  while (getline(file, fileLine)) {
    size_t lineSize = fileLine.length();

    // Determine the current list

    // If the line contains the size of the container list
    if (lineSize <= 10) {
      currVectorSize = stoi(fileLine);

      currList = isFirstCont ? regis : fiscs;

      // Allocate memory for the container list
      (*currList)->size = currVectorSize;
      (*currList)->list = new Container[currVectorSize];

      isFirstCont = false;
      // Switch to the next container list
      currIndex = 0;
      continue;
    }

    // Process a line with container details
    string newCnpj, newCode, newWeight;
    int newWeightNumeric, propStatus = 0;
    string *props[3] = {&newCode, &newCnpj, &newWeight};

    for (size_t i = 0; i < lineSize; i++) {
      writeContainerProp(props, fileLine.at(i), propStatus);
    }
    newWeightNumeric = stoi(newWeight);

    // Validate if the current list's memory is allocated
    if ((*currList)->list == nullptr) {
      cerr << "Error: Memory not allocated for container list!" << endl;
      return EXIT_FAILURE;
    }

    // Write properties into the allocated container list
    (*currList)->list[currIndex].code = newCode;
    (*currList)->list[currIndex].cnpj = newCnpj;
    (*currList)->list[currIndex].weight = newWeightNumeric;

    currIndex++;
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[3]) {
  ifstream input(argv[1]);  // Input File
  ofstream output(argv[2]); // Output file

  if (!input.is_open()) {
    cerr << "erro ao abrir input" << endl;
    return EXIT_FAILURE;
  }

  if (!output.is_open()) {
    cerr << "erro ao abrir output" << endl;
    return EXIT_FAILURE;
  }

  ContainerList *registeredContainers = nullptr;
  ContainerList *fiscalizedContainers = nullptr;
  readInputAndCreateContainerLists(input, &registeredContainers,
                                   &fiscalizedContainers);

  unordered_map<string, Container> fiscalizedsMap =
      createContainerMap(fiscalizedContainers, fiscalizedContainers->size);

  ContainerList *duplicatedContainers = filterRegisteredContainersForInspection(
      registeredContainers, registeredContainers->size, fiscalizedsMap);

  IrregularList *irregulars = createIrregularContainersList(
      fiscalizedsMap, duplicatedContainers, duplicatedContainers->size);

  sortIrregularContainers(irregulars->list, fiscalizedsMap, 0,
                          irregulars->size - 1);

  for (int i = 0; i < irregulars->size; i++) {
    output << irregulars->list[i].irregularityMessage << endl;
  }
  return EXIT_SUCCESS;
}

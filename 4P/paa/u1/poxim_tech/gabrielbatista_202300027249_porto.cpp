#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>

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

int calculateCode(string str) {
  int code = 0;
  for (char character : str)
    code += character;
  return code;
}

bool isGreaterThan(float a, float b, float e = 1e-6) {
  return (a > b) && fabs(a - b) > e;
}

float calculateDifPercent(int n1, int n2) {
  int dif = n1 - n2;
  float percentage = (float)dif / n1 * 100;
  return abs(percentage);
}

void merge(Container *list, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  // Alocando memória para os subarrays temporários
  Container *leftArr = new Container[n1];
  Container *rightArr = new Container[n2];

  // Copiando os dados para os subarrays temporários
  for (int i = 0; i < n1; i++)
    leftArr[i] = list[left + i];
  for (int j = 0; j < n2; j++)
    rightArr[j] = list[mid + 1 + j];

  // Mesclando os subarrays temporários de volta ao array original
  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (leftArr[i].code <= rightArr[j].code) {
      list[k] = leftArr[i];
      i++;
    } else {
      list[k] = rightArr[j];
      j++;
    }
    k++;
  }

  // Copiando os elementos restantes de leftArr, se houver
  while (i < n1) {
    list[k] = leftArr[i];
    i++;
    k++;
  }

  // Copiando os elementos restantes de rightArr, se houver
  while (j < n2) {
    list[k] = rightArr[j];
    j++;
    k++;
  }

  // Liberando a memória alocada
  delete[] leftArr;
  delete[] rightArr;
}

// Função recursiva para implementar o Merge Sort
void sortInAlphabeticOrder(Container *list, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    // Ordenando a primeira e a segunda metade
    sortInAlphabeticOrder(list, left, mid);
    sortInAlphabeticOrder(list, mid + 1, right);

    // Mesclando as duas metades
    merge(list, left, mid, right);
  }
}

Container *binarySearch(Container *list, string code, int left, int right,
                        int i = 0) {
  if (left > right) {
    return nullptr;
  }
  int mid = left + (right - left) / 2;
  if (code < list[mid].code) {
    return binarySearch(list, code, 0, mid - 1, i + 1);
  } else if (code > list[mid].code) {
    return binarySearch(list, code, mid + 1, right, i + 1);
  } else {
    return &list[mid];
  }
}
ContainerList *
filterRegisteredContainersForInspection(ContainerList *registereds,
                                        int registeredsSize,
                                        Container *fiscalizeds, int fiscSize) {
  ContainerList *duplicateds = new ContainerList[1];
  if (!duplicateds) {
    return nullptr;
  }
  duplicateds->list = new Container[fiscSize];
  duplicateds->size = 0;
  int j = 0;
  for (int i = 0; i < registeredsSize; i++) {
    // Verifies if the registered container was fiscalized:
    Container *dupli =
        binarySearch(fiscalizeds, registereds->list[i].code, 0, fiscSize - 1);
    if (dupli == nullptr) {
      continue;
    } else if (registereds->list[i].code == dupli->code) {
      duplicateds->list[j++] = registereds->list[i];
    }
  }
  duplicateds->size = j;
  return duplicateds;
}
bool hasDifferentCnjp(Container container, Container *fiscalizeds, string &msg,
                      int fiscSize) {
  Container *fiscalized =
      binarySearch(fiscalizeds, container.code, 0, fiscSize - 1);
  if (fiscalized == nullptr)
    return false;
  if (container.code == fiscalized->code &&
      container.cnpj != fiscalized->cnpj) {
    msg = container.cnpj + "<->" + fiscalized->cnpj;
    return true;
  }
  return false;
}
float calcContainerWeightDifPercent(Container container, Container *fiscalizeds,
                                    int fiscSize,
                                    float *bruteWeightDif = nullptr) {

  Container *fiscalized =
      binarySearch(fiscalizeds, container.code, 0, fiscSize);
  if (fiscalized == nullptr)
    return EXIT_FAILURE;
  if (container.code == fiscalized->code) {
    float weightDif = calculateDifPercent(container.weight, fiscalized->weight);
    if (bruteWeightDif != nullptr)
      *bruteWeightDif = fabs(container.weight - fiscalized->weight);
    return weightDif;
  }
  return EXIT_SUCCESS;
}
IrregularList *createIrregularContainersList(Container *fiscalizeds,
                                             ContainerList *registereds,
                                             int fiscSize, int regisSize) {
  string irrMsg;
  IrregularList *irregulars = new IrregularList[1];
  irregulars->size = 0;

  Irregular *irrList = new Irregular[1];
  irregulars->list = irrList;

  for (int i = 0; i < regisSize; i++) {
    Container registered = registereds->list[i];
    if (hasDifferentCnjp(registered, fiscalizeds, irrMsg, fiscSize)) {
      Irregular newCnpjIrr;
      newCnpjIrr.code = registered.code;
      newCnpjIrr.cnpj = registered.cnpj;
      newCnpjIrr.weight = registered.weight;
      newCnpjIrr.irregularity = CNPJ;
      newCnpjIrr.irregularityMessage = registered.code + ":" + irrMsg;
      irregulars->size++;
      Irregular *resizedIrrList = new Irregular[irregulars->size];
      for (int i = 0; i < irregulars->size - 1; i++) {
        resizedIrrList[i] = irregulars->list[i];
      }
      irregulars->list = resizedIrrList;
      irregulars->list[irregulars->size - 1] = newCnpjIrr;
    } else {
      float weightDif;
      float weightDifPercent = calcContainerWeightDifPercent(
          registered, fiscalizeds, fiscSize, &weightDif);
      if (isGreaterThan(weightDifPercent, 10)) {
        ostringstream difPerc, bruteDif;
        difPerc << fixed << setprecision(0) << weightDifPercent;
        bruteDif << fixed << setprecision(0) << weightDif;
        string irrMsg = bruteDif.str() + "kg(" + difPerc.str() + "%)";
        Irregular newWeightIrr;
        newWeightIrr.code = registered.code;
        newWeightIrr.cnpj = registered.cnpj;
        newWeightIrr.weight = registered.weight;
        newWeightIrr.irregularity = WEIGHT;
        newWeightIrr.irregularityMessage = registered.code + ":" + irrMsg;
        irregulars->size++;
        Irregular *resizedIrrList = new Irregular[irregulars->size];
        for (int i = 0; i < irregulars->size - 1; i++) {
          resizedIrrList[i] = irregulars->list[i];
        }
        irregulars->list = resizedIrrList;
        irregulars->list[irregulars->size - 1] = newWeightIrr;
      }
    }
  }
  return irregulars;
}
int mergeIrregularContainers(Irregular *irregulars, Container *fiscalizeds,
                             int fiscSize, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;
  Irregular leftVec[n1], rightVec[n2];
  for (int a = 0; a < n1; a++)
    leftVec[a] = irregulars[left + a];
  for (int b = 0; b < n2; b++)
    rightVec[b] = irregulars[mid + 1 + b];
  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    Irregularity leftIrregularity = leftVec[i].irregularity;
    Irregularity rightIrregularity = rightVec[j].irregularity;
    if (leftIrregularity > rightIrregularity) {
      irregulars[k++] = leftVec[i++];
    } else if (leftIrregularity == rightIrregularity) {
      if (leftIrregularity == WEIGHT) {
        int weightIrrL = round(
            calcContainerWeightDifPercent(leftVec[i], fiscalizeds, fiscSize));
        int weightIrrR = round(
            calcContainerWeightDifPercent(rightVec[j], fiscalizeds, fiscSize));
        if (isGreaterThan(weightIrrL, weightIrrR) || weightIrrL == weightIrrR) {
          irregulars[k++] = leftVec[i++];
        } else {
          irregulars[k++] = rightVec[j++];
        }
      } else {
        irregulars[k++] = leftVec[i++];
      }
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

void sortIrregularContainers(Irregular *irregulars, Container *fiscalizeds,
                             int fiscSize, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    sortIrregularContainers(irregulars, fiscalizeds, fiscSize, left, mid);
    sortIrregularContainers(irregulars, fiscalizeds, fiscSize, mid + 1, right);
    mergeIrregularContainers(irregulars, fiscalizeds, fiscSize, left, mid,
                             right);
  }
}
int readInputAndCreateContainerLists(ifstream &file, ContainerList **regis,
                                     ContainerList **fiscs) {
  string fileLine;
  int currIndex = 0;
  int currVectorSize = 0;
  bool isFirstCont = true;
  *regis = new ContainerList();
  (*regis)->size = 0;
  (*regis)->list = nullptr;

  *fiscs = new ContainerList();
  (*fiscs)->size = 0;
  (*fiscs)->list = nullptr;
  ContainerList **currList = nullptr;
  while (getline(file, fileLine)) {
    size_t lineSize = fileLine.length();
    if (lineSize <= 10) {
      currVectorSize = stoi(fileLine);
      currList = isFirstCont ? regis : fiscs;
      (*currList)->size = currVectorSize;
      (*currList)->list = new Container[currVectorSize];
      isFirstCont = false;
      currIndex = 0;
      continue;
    }
    string newCnpj, newCode, newWeight;
    int newWeightNumeric;
    istringstream iss(fileLine);
    iss >> newCode >> newCnpj >> newWeight;
    newWeightNumeric = stoi(newWeight);
    if ((*currList)->list == nullptr) {
      cerr << "Error: Memory not allocated for container list!" << endl;
      return EXIT_FAILURE;
    }
    (*currList)->list[currIndex].code = newCode;
    (*currList)->list[currIndex].cnpj = newCnpj;
    (*currList)->list[currIndex].weight = newWeightNumeric;
    currIndex++;
  }
  return EXIT_SUCCESS;
}
int main(int argc, char *argv[3]) {
  ifstream input(argv[1]);
  ofstream output(argv[2]);
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
  sortInAlphabeticOrder(fiscalizedContainers->list, 0,
                        fiscalizedContainers->size - 1);
  IrregularList *irregulars = createIrregularContainersList(
      fiscalizedContainers->list, registeredContainers,
      fiscalizedContainers->size, registeredContainers->size);
  sortIrregularContainers(irregulars->list, fiscalizedContainers->list,
                          fiscalizedContainers->size, 0, irregulars->size - 1);
  for (int i = 0; i < irregulars->size; i++) {
    output << irregulars->list[i].irregularityMessage << endl;
  }
  return EXIT_SUCCESS;
}

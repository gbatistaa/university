#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
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

ContainerList *
filterRegisteredContainersForInspection(ContainerList *registereds,
                                        int registeredsSize,
                                        Container *fiscalizedsBST) {
  ContainerList *duplicateds = new ContainerList[1];
  if (!duplicateds) {
    cerr << "Erro na alocação de memória!" << endl;
    return nullptr;
  }
  duplicateds->list = nullptr;
  duplicateds->size = 0;

  Container *newDuplicatedsList = new Container[1];
  duplicateds->list = newDuplicatedsList;
  for (int i = 0; i < registeredsSize; i++) {
    try {
      // Verifies if the registered container was fiscalized:
      if (binarySearch(fiscalizedsBST, registereds->list[i]) == nullptr)
        continue;
      if (registereds->list[i].code ==
          binarySearch(fiscalizedsBST, registereds->list[i])->value.code) {

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

bool hasDifferentCnjp(Container container, Container *fiscalizeds,
                      string &msg) {
  Container fiscalized = binarySearch(fiscalizeds, container)->value;
  if (container.code == fiscalized.code && container.cnpj != fiscalized.cnpj) {
    msg = container.cnpj + "<->" + fiscalized.cnpj;
    return true;
  }
  return false;
}
float calcContainerWeightDifPercent(Container container, Container *fiscalizeds,
                                    float *bruteWeightDif = nullptr) {
  try {
    Container fiscalized = binarySearch(fiscalizeds, container)->value;
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
IrregularList *createIrregularContainersList(Container *fiscalizeds,
                                             ContainerList *duplicateds,
                                             int duplicatedsSize) {
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

int mergeIrregularContainers(Irregular *irregulars, Container *fiscalizeds,
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
void sortIrregularContainers(Irregular *irregulars, Container *fiscalizeds,
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

    // If the line contains the size of the container list
    if (lineSize <= 10) {
      currVectorSize = stoi(fileLine);

      // Determine the current list
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

    istringstream iss(fileLine);
    iss >> newCode >> newCnpj >> newWeight;

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

  cout << "Arquivo lido com sucesso!\n" << endl;

  cout << " criada com sucesso!" << endl;

  ContainerList *duplicatedContainers = filterRegisteredContainersForInspection(
      registeredContainers, registeredContainers->size,
      fiscalizedContainers->list);

  cout << "Containeres duplicados filtrados com sucesso!" << endl;

  IrregularList *irregulars = createIrregularContainersList(
      fiscalizedContainers->list, duplicatedContainers,
      duplicatedContainers->size);

  cout << "Lista de containeres irregulares criada com sucesso!\n" << endl;

  sortIrregularContainers(irregulars->list, fiscalizedContainers->list, 0,
                          irregulars->size - 1);

  cout << "Ordenação da lista de irregulares criada com sucesso!\n";

  for (int i = 0; i < irregulars->size; i++) {
    output << irregulars->list[i].irregularityMessage << endl;
  }
  cout << "Fim" << endl;
  return EXIT_SUCCESS;
}

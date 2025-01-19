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
class bst {

public:
  bst() { root = nullptr; };

  class Node {
  public:
    Container value;
    Node *leftChild;
    Node *rightChild;

    Node() : value(Container()), leftChild(nullptr), rightChild(nullptr) {}
  };

  Node *root;

  Node *createNode(Container container) {
    Node *newPtr = new Node;
    if (newPtr == nullptr) {
      cerr << "Error on memory allocation for new node" << endl;
      return nullptr;
    }

    newPtr->value = container;
    newPtr->leftChild = nullptr;
    newPtr->rightChild = nullptr;
    return newPtr;
  }

  Node *insert(Container newContainer, Node *root) {
    if (root == nullptr) {
      Node *newNode = createNode(newContainer);
      return newNode;
    }
    int newNodeCode = calculateCode(newContainer.code);
    if (calculateCode(root->value.code) < newNodeCode)
      root->leftChild = insert(newContainer, root->leftChild);
    else if (calculateCode(root->value.code) > newNodeCode)
      root->rightChild = insert(newContainer, root->rightChild);
    else {
      Node *currNode = root;
      while (currNode != nullptr) {
        if (currNode->value.code == newContainer.code)
          return root;
        currNode = currNode->leftChild;
      }
    }
    return root;
  }

  Node *insertIte(Container newContainer, Node *root) {
    Node **current = &root;
    int newNodeCode = calculateCode(newContainer.code);
    while (*current != nullptr) {
      int currentCode = calculateCode((*current)->value.code);
      if (currentCode < newNodeCode)
        current = &(*current)->leftChild;
      else if (currentCode > newNodeCode)
        current = &(*current)->rightChild;
      else {
      }
    }
    *current = createNode(newContainer);
    return root;
  }

  Node *at(Node *root, Container value) {
    Node *current = root;
    while (current != nullptr) {
      int code = calculateCode(value.code);
      int currentCode = calculateCode(current->value.code);
      if (currentCode < code) {
        current = current->leftChild;
      } else if (currentCode > code) {
        current = current->rightChild;
      } else {
        Node *currNode = current;
        while (currNode != nullptr) {
          if (currNode->value.code == value.code) {
            return currNode;
          }
          currNode = currNode->leftChild;
        }
        return nullptr;
      }
    }
    return nullptr;
  }

private:
  int calculateCode(string str) {
    int code = 0;
    for (char character : str)
      code += character;
    return code;
  }
};

bool isGreaterThan(float a, float b, float e = 1e-6) {
  return (a > b) && fabs(a - b) > e;
}

float calculateDifPercent(int n1, int n2) {
  int dif = n1 - n2;
  float percentage = (float)dif / n1 * 100;
  return abs(percentage);
}

ContainerList *filterRegisteredContainersForInspection(
    ContainerList *registereds, int registeredsSize, bst *fiscalizedsBST) {
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
      if (fiscalizedsBST->at(fiscalizedsBST->root, registereds->list[i]) ==
          nullptr)
        continue;
      if (registereds->list[i].code ==
          fiscalizedsBST->at(fiscalizedsBST->root, registereds->list[i])
              ->value.code) {

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
bst *createContainerBST(ContainerList *fiscalizeds, int fiscSize) {
  bst *newContainerBST = new bst();

  for (int i = 0; i < fiscSize; i++) {
    newContainerBST->root =
        newContainerBST->insert(fiscalizeds->list[i], newContainerBST->root);
  }
  return newContainerBST;
}
bool hasDifferentCnjp(Container container, bst *fiscalizeds, string &msg) {
  Container fiscalized = fiscalizeds->at(fiscalizeds->root, container)->value;
  if (container.code == fiscalized.code && container.cnpj != fiscalized.cnpj) {
    msg = container.cnpj + "<->" + fiscalized.cnpj;
    return true;
  }
  return false;
}
float calcContainerWeightDifPercent(Container container, bst *fiscalizeds,
                                    float *bruteWeightDif = nullptr) {
  try {
    Container fiscalized = fiscalizeds->at(fiscalizeds->root, container)->value;
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
IrregularList *createIrregularContainersList(bst *fiscalizeds,
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

int mergeIrregularContainers(Irregular *irregulars, bst *fiscalizeds, int left,
                             int mid, int right) {
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
void sortIrregularContainers(Irregular *irregulars, bst *fiscalizeds, int left,
                             int right) {
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

  bst *fiscalizedsBST =
      createContainerBST(fiscalizedContainers, fiscalizedContainers->size);

  cout << "Árvore binária criada com sucesso!" << endl;
  cout << "Código do container da raiz: " + fiscalizedsBST->root->value.code +
              "\n"
       << endl;

  ContainerList *duplicatedContainers = filterRegisteredContainersForInspection(
      registeredContainers, registeredContainers->size, fiscalizedsBST);

  cout << "Containeres duplicados filtrados com sucesso!" << endl;

  IrregularList *irregulars = createIrregularContainersList(
      fiscalizedsBST, duplicatedContainers, duplicatedContainers->size);

  cout << "Lista de containeres irregulares criada com sucesso!\n" << endl;

  sortIrregularContainers(irregulars->list, fiscalizedsBST, 0,
                          irregulars->size - 1);

  cout << "Ordenação da lista de irregulares criada com sucesso!\n";

  for (int i = 0; i < irregulars->size; i++) {
    output << irregulars->list[i].irregularityMessage << endl;
  }
  cout << "Fim" << endl;
  return EXIT_SUCCESS;
}

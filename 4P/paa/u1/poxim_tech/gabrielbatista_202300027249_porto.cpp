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

using namespace std;

template <typename Value> class hashmap {
private:
  int elements_num;
  float density;

  typedef struct key_value {
    string key;
    Value value;
  } key_value;

  class linked_pair {
  public:
    key_value pair;
    linked_pair *next;
  };

  bool isGreaterThan(float a, float b, float e = 1e-6) {
    return (a > b) && fabs(a - b) > e;
  }

  int re_hash() {
    // Doubleing the size of the hashmap array:
    int prev_map_size = map_size;
    map_size = (int)map_size * sqrt(5);
    linked_pair *new_map = (linked_pair *)calloc(map_size, sizeof(linked_pair));

    // Re-hashing all the elements:
    for (int i = 0; i < prev_map_size; i++) {

      // Ignoring the empty spaces:
      if (map[i].pair.key == "")
        continue;

      // Copying the current previous map element:
      linked_pair *curr_copy = &map[i];
      while (curr_copy != nullptr) {

        int hash_index = hash_function(curr_copy->pair.key, map_size);
        linked_pair *curr_pair = &new_map[hash_index];

        // Verifying if already has element in the hash index:
        if (curr_pair->pair.key != "") {
          linked_pair *copy_pair = (linked_pair *)malloc(sizeof(linked_pair));
          copy_pair->pair = curr_copy->pair;
          copy_pair->next = nullptr;

          // Scanning until it reaches the last element of the linked list:
          while (curr_pair->next != nullptr) {
            curr_pair = curr_pair->next;
          }
          curr_pair->next = copy_pair;
        } else {
          curr_pair->pair = curr_copy->pair;
          curr_pair->next = nullptr;
        }

        // Passing to the next element of the previous map colision linked list:
        curr_copy = curr_copy->next;
      }
    }
    map = new_map;

    return EXIT_SUCCESS;
  }

  int hash_function(string key, int list_size) {
    int hash_index = 0;
    for (char letter : key)
      hash_index += letter;
    return hash_index % list_size;
  }

public:
  int map_size;

  // Hash Map constructor:
  hashmap() {
    elements_num = 0;
    map_size = 2;
    map = (linked_pair *)calloc(map_size, sizeof(linked_pair));
  };

  linked_pair *map;

  linked_pair *hash_insert(string key, Value value) {
    elements_num++;

    // Storing the new key value pair:
    key_value kv;
    kv.key = key;
    kv.value = value;

    linked_pair *new_pair = (linked_pair *)malloc(sizeof(linked_pair));
    new_pair->pair = kv;
    new_pair->next = nullptr;

    // Case when the hashmap has 0 elements
    density = (float)elements_num / map_size;

    // Condition to verify if the resizing is needed (more than 75%):
    if (isGreaterThan(density, 0.75)) {
      re_hash();
    }

    int hash_index = hash_function(key, map_size);

    // Case that the hash index collides:
    if (map[hash_index].pair.key != "") {
      linked_pair *curr_pair = &map[hash_index];
      while (curr_pair->next != nullptr && curr_pair->pair.key != key) {
        curr_pair = curr_pair->next;
      }
      if (curr_pair->next == nullptr) {
        curr_pair->next = new_pair;
      } else {
        curr_pair->next->pair.value = value;
      }
    } else {
      map[hash_index] = *new_pair;
    }
    return map;
  }

  int print_hash_map() {
    for (int i = 0; i < map_size; i++) {
      if (map[i].pair.key != "") {
        cout << map[i].pair.key << " | " << map[i].pair.value;
        linked_pair *curr_pair = &map[i];

        // Scanning all of the collided pairs:
        while (curr_pair->next != nullptr && curr_pair->next->pair.key != "") {
          cout << " --> " << curr_pair->next->pair.key << " | "
               << curr_pair->next->pair.value;
          curr_pair = curr_pair->next;
        }
        cout << endl;
      }
    }
    return EXIT_SUCCESS;
  }

  Value at(string key) {
    int hash_index = hash_function(key, map_size);

    if (map[hash_index].pair.key == key)
      return map[hash_index].pair.value;
    else {
      linked_pair *curr_pair = &map[hash_index];
      try {
        while (curr_pair != nullptr) {
          if (curr_pair->pair.key == key) {
            return curr_pair->pair.value;
          }
          curr_pair = curr_pair->next;
        }

        // If reaches a null pointer the key were not created:
        throw runtime_error("Key not found");
      } catch (const exception &e) {
        return Value();
      }
    }
    return Value();
  }
};

enum Irregularity {
  WEIGHT,
  CNPJ,
};

class Container {
public:
  string code;
  string cnpj;
  int weight;
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

ContainerList *
filterRegisteredContainersForInspection(ContainerList *registereds,
                                        int registeredsSize,
                                        hashmap<Container> fiscalizedsMap) {
  ContainerList *duplicateds = (ContainerList *)malloc(sizeof(ContainerList));
  if (!duplicateds) {
    exit(EXIT_FAILURE);
  }
  duplicateds->list = nullptr;
  duplicateds->size = 0;

  Container *newDuplicatedsList = (Container *)malloc(sizeof(Container));
  duplicateds->list = newDuplicatedsList;
  for (int i = 0; i < registeredsSize; i++) {
    try {
      // Verifies if the registered container was fiscalized:
      if (registereds->list[i].code ==
          fiscalizedsMap.at(registereds->list[i].code).code) {

        // Increments the duplicated containers list size:

        duplicateds->size++;
        Container *resizedList = (Container *)realloc(
            duplicateds->list, duplicateds->size * sizeof(Container));

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

hashmap<Container> createContainerMap(ContainerList *fiscalizeds,
                                      int fiscSize) {
  hashmap<Container> newContainerMap;
  for (int i = 0; i < fiscSize; i++) {
    newContainerMap.hash_insert(fiscalizeds->list[i].code,
                                fiscalizeds->list[i]);
  }
  return newContainerMap;
}

bool hasDifferentCnjp(Container container, hashmap<Container> fiscalizeds,
                      string &msg) {
  Container fiscalized = fiscalizeds.at(container.code);
  if (container.code == fiscalized.code && container.cnpj != fiscalized.cnpj) {
    msg = container.cnpj + "<->" + fiscalized.cnpj;
    return true;
  }
  return false;
}

float calcContainerWeightDifPercent(Container container,
                                    hashmap<Container> fiscalizeds,
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

IrregularList *createIrregularContainersList(hashmap<Container> fiscalizeds,
                                             ContainerList *duplicateds,
                                             int duplicatedsSize) {
  string irrMsg;
  IrregularList *irregulars = (IrregularList *)malloc(sizeof(IrregularList));
  irregulars->size = 0;
  irregulars->list = nullptr;

  Irregular *irrList = (Irregular *)malloc(sizeof(Irregular));
  irregulars->list = irrList;

  for (int i = 0; i < duplicatedsSize; i++) {
    Container duplicated = duplicateds->list[i];

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
      newCnpjIrr.irregularityMessage = duplicated.code + ":" + irrMsg;

      irregulars->size++;

      Irregular *resizedIrrList = (Irregular *)realloc(
          irregulars->list, sizeof(Irregular) * irregulars->size);
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

      Irregular *resizedIrrList = (Irregular *)realloc(
          irregulars->list, sizeof(Irregular) * irregulars->size);
      irregulars->list = resizedIrrList;
      irregulars->list[irregulars->size - 1] = newWeightIrr;
    }
  }

  return irregulars;
}

int mergeIrregularContainers(Irregular *irregulars,
                             hashmap<Container> fiscalizeds, int left, int mid,
                             int right) {
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
    if (leftIrregularity >= rightIrregularity) {
      irregulars[k++] = leftVec[i];
      i++;
    } else {
      irregulars[k++] = rightVec[j];
      j++;
    }
  }

  while (i < n1) {
    irregulars[k] = leftVec[i];
    i++;
    k++;
  }

  while (j < n2) {
    irregulars[k] = rightVec[j];
    j++;
    k++;
  }
  return EXIT_SUCCESS;
}

// Função de ordenação Merge Sort
void sortIrregularContainers(Irregular *irregulars,
                             hashmap<Container> fiscalizeds, int left,
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

int readInputAndCreateContainerLists(fstream &file,
                                     ContainerList **contPointers) {
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

    // Writing the colected data in the props of the new container:
    newContainer.code = newCode;
    newContainer.cnpj = newCnpj;
    newContainer.weight = newWeightNumeric;

    // Incrementing the new containers list size and storing the previous value:
    int prevListSize = contPointers[currContainerIndex]->size;

    contPointers[currContainerIndex]->size++;

    int newListSize = contPointers[currContainerIndex]->size;

    // Adding the new container in the containers list:
    Container *newArray = (Container *)malloc(sizeof(Container) * newListSize);
    if (newArray == NULL) {
      return EXIT_FAILURE;
    }

    for (int x = 0; x < prevListSize; x++) {
      newArray[x] = contPointers[currContainerIndex]->list[x];
    }

    // Reallocating the pointer to the new array:
    contPointers[currContainerIndex]->list = newArray;

    // Adding the new element to the last position of the container list:
    contPointers[currContainerIndex]->list[newListSize - 1] = newContainer;
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[3]) {
  fstream input(argv[1]);  // Input File
  fstream output(argv[2]); // Output file

  if (!input.is_open()) {
    cerr << "erro" << endl;
    return EXIT_FAILURE;
  }

  if (!output.is_open()) {
    cerr << "erro" << endl;
    return EXIT_FAILURE;
  }

  ContainerList *registeredContainers =
      (ContainerList *)malloc(sizeof(ContainerList));
  registeredContainers->size = 0;
  registeredContainers->list =
      (Container *)malloc(sizeof(Container) * registeredContainers->size);

  ContainerList *fiscalizedContainers =
      (ContainerList *)malloc(sizeof(ContainerList));
  fiscalizedContainers->size = 0;
  fiscalizedContainers->list =
      (Container *)malloc(sizeof(Container) * fiscalizedContainers->size);

  ContainerList *containersPointers[2] = {registeredContainers,
                                          fiscalizedContainers};

  readInputAndCreateContainerLists(input, containersPointers);

  hashmap<Container> fiscalizedsMap =
      createContainerMap(fiscalizedContainers, fiscalizedContainers->size);

  ContainerList *duplicatedContainers = filterRegisteredContainersForInspection(
      registeredContainers, registeredContainers->size, fiscalizedsMap);

  IrregularList *irregulars = createIrregularContainersList(
      fiscalizedsMap, duplicatedContainers, duplicatedContainers->size);

  sortIrregularContainers(irregulars->list, fiscalizedsMap, 0,
                          irregulars->size - 1);

  for (int i = 0; i < irregulars->size; i++) {
    output << irregulars->list[i].irregularityMessage << endl;
    cout << irregulars->list[i].irregularityMessage << endl;
  }
  return EXIT_SUCCESS;
}

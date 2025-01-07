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

// int mergeContainers(vector<Container> registereds,
//                     vector<Container> inspectioneds,
//                     vector<Container> &irregulars, int left, int mid,
//                     int right) {

//   int n1 = mid - left + 1;
//   int n2 = right - mid;

//   vector<Container> leftSubVector, rightSubVector;

//   for (int i = 0; i < n1; i++)
//     leftSubVector.push_back(registereds[left + i]);
//   for (int j = 0; j < n2; j++)
//     rightSubVector.push_back(registereds[mid + 1 + j]);

//   int i = 0;
//   int j = 0;
//   int k = left;
//   Container containerLeft, containerRight;

//   // Sorting the containers by CNPJ and weight diference:
//   while (i < n1 && j < n2) {
//     containerLeft = leftSubVector[i];
//     containerRight = rightSubVector[j];

//     // Case 1: Both containers has different CNPJ
//     if (hasDifferentCnjp(containerLeft, inspectioneds) &&
//         hasDifferentCnjp(containerRight, inspectioneds)) {
//       irregulars[k] = containerLeft;
//       k++;
//       irregulars[k] = containerRight;
//       k++;
//       i++;
//       j++;
//     }

//     // Case 2: Only one of the containers has different CNPJ
//     else if (hasDifferentCnjp(containerLeft, inspectioneds)) {
//       irregulars[k] = containerLeft;
//       cout << containerLeft.code << " | " << containerLeft.cnpj << " left"
//            << "\n";
//       i++;
//       k++;
//     } else if (hasDifferentCnjp(containerRight, inspectioneds)) {
//       irregulars[k] = containerRight;
//       cout << containerRight.code << " | " << containerRight.cnpj << "right"
//            << "\n";
//       j++;
//       k++;
//     }

//     // Case 3: None of the containers has different registered CNPJ
//     else {
//       float difContL =
//           calcContainerWeightDifPercent(containerLeft, inspectioneds);
//       float difContR =
//           calcContainerWeightDifPercent(containerRight, inspectioneds);

//       // Case 3.1: Both containers has more than 10% of inspectioned
//       // difference
//       if (isGreaterThan(difContL, 10) && isGreaterThan(difContR, 10)) {

//         // Case 3.1.1: Both containers has more than 10% of inspectioned
//         // difference and are equals
//         if (areFloatsEqual(difContL, difContR)) {
//           irregulars[k] = containerLeft;
//           i++;
//           k++;

//           // Case 3.1.2: Both has more than 10% of inspectioned
//           // difference but left container weight is bigger
//         } else if (isGreaterThan(difContL, difContR)) {
//           irregulars[k] = containerLeft;
//           i++;
//           k++;
//         }
//         // Case 3.1.3: Both has more than 10% of inspectioned
//         // difference but right container weight is bigger
//         else if (isLessThan(difContL, difContR)) {
//           irregulars[k] = containerRight;
//           j++;
//           k++;
//         }

//         // Case 3.2: Only one of the containers are available for inspection
//       } else if (isGreaterThan(difContL, 10)) {
//         irregulars[k] = containerLeft;
//         i++;
//         k++;
//       } else if (isGreaterThan(difContR, 10)) {
//         irregulars[k] = containerRight;
//         j++;
//         k++;
//       }
//     }
//   }

// Wrtiting the remaining containers on the vector :
// while (j < n2) {
//   if (hasDifferentCnjp(containerLeft, inspectioneds) ||
//       isGreaterThan(
//           calcContainerWeightDifPercent(containerLeft, inspectioneds), 10))
//           {
//     containerRight = rightSubVector[j];
//     irregulars[k] = containerRight;
//     k++;
//   }
//   j++;
// }
// while (i < n1) {
//   if (hasDifferentCnjp(containerLeft, inspectioneds) ||
//       isGreaterThan(
//           calcContainerWeightDifPercent(containerLeft, inspectioneds), 10))
//           {
//     containerLeft = leftSubVector[i];
//     irregulars[k] = containerLeft;
//     k++;
//   }
//   i++;
// }

//   return EXIT_SUCCESS;
// }

// If this algorithym works pass it to a estableized version:
// int sortContainersForInspection(vector<Container> registereds,
//                                 vector<Container> inspectioneds,
//                                 vector<Container> &irregulars, int left,
//                                 int right) {
//   if (left < right) {
//     int mid = left + (right - left) / 2;

//     sortContainersForInspection(registereds, inspectioneds, irregulars, left,
//                                 mid);
//     sortContainersForInspection(registereds, inspectioneds, irregulars, mid +
//     1,
//                                 right);
//     mergeContainers(registereds, inspectioneds, irregulars, left, mid,
//     right);
//   }
//   return EXIT_SUCCESS;
// }

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

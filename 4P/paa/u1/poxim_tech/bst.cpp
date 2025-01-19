#include <complex.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdbool.h>

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
    Node *newNode = createNode(newContainer);
    if (root == nullptr) {
      return newNode;
    }
    int newNodeCode = calculateCode(newNode->value.code);
    if (calculateCode(root->value.code) < newNodeCode)
      root->leftChild = insert(newContainer, root->leftChild);
    else if (calculateCode(root->value.code) > newNodeCode)
      root->rightChild = insert(newContainer, root->rightChild);
    else {
      if (root->value.code == newContainer.code)
        return root;
      else {
        Node *currNode = root;
        while (currNode->leftChild != nullptr) {
          currNode = currNode->leftChild;
        }
        currNode->leftChild = newNode;
      }
    };
    return root;
  }

  bool binarySearch(Node *root, Container newValue) {
    if (root == nullptr)
      return false;
    int code = calculateCode(newValue.code);
    if (calculateCode(root->value.code) < code)
      return binarySearch(root->leftChild, newValue);
    else if (calculateCode(root->value.code) > code)
      return binarySearch(root->rightChild, newValue);
    else {
      Node *currNode = root;
      while (currNode != nullptr) {
        if (currNode->value.code == newValue.code) {
          return true;
        }
        currNode = currNode->leftChild;
      }
      return false;
    };
  }

private:
  int calculateCode(string str) {
    int code = 0;
    for (char character : str)
      code += character;
    return code;
  }

  Node *root;
};

int main() {
  // Cria instância da árvore
  bst tree;
  bst::Node *root = nullptr;

  // Criação manual dos containers
  Container c1;
  c1.code = "ABC";
  c1.cnpj = "12345678";
  c1.weight = 50;

  Container c2;
  c2.code = "DEF";
  c2.cnpj = "87654321";
  c2.weight = 70;

  Container c3;
  c3.code = "GHI";
  c3.cnpj = "11223344";
  c3.weight = 30;

  Container c4;
  c4.code = "JKL";
  c4.cnpj = "44332211";
  c4.weight = 90;

  Container c5;
  c5.code = "MNO";
  c5.cnpj = "56789012";
  c5.weight = 40;

  Container c6;
  c6.code = "AOZ";
  c6.cnpj = "19825628";
  c6.weight = 192;

  // Inserção na árvore
  root = tree.insert(c1, root);
  root = tree.insert(c2, root);
  root = tree.insert(c3, root);
  root = tree.insert(c4, root);
  root = tree.insert(c5, root);
  root = tree.insert(c6, root);

  cout << (tree.binarySearch(root, c6)
               ? "Container com o código (" + c6.code + ") foi encontrado!"
               : "Container não encontrado!")
       << endl;

  return EXIT_SUCCESS;
}

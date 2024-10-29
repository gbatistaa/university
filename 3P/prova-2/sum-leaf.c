#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
  int value;
  struct node *leftChild;
  struct node *rightChild;
} BinaryNode;

BinaryNode *createBinaryNode(int value) {
  BinaryNode *newNode = (BinaryNode*)malloc(sizeof(BinaryNode));
  newNode->value = value;
  newNode->leftChild = NULL;
  newNode->rightChild = NULL;
  return newNode;
}

int printInOrder(BinaryNode *root) {
  if(root == NULL) {
    return EXIT_SUCCESS;
  } else {
    printInOrder(root->leftChild);
    printf("%d", root->value);
    printInOrder(root->rightChild);
  }
}

BinaryNode *insertBinaryNode(BinaryNode *root, int value) {
  BinaryNode *newNode = createBinaryNode(value);

  if (root == NULL) {
    return newNode;
  }

  if (value < root->value) {
    root->leftChild = insertBinaryNode(root->leftChild, value);
  } else if (value > root->value) {
    root->rightChild = insertBinaryNode(root->rightChild, value);
  }

  return root;
}

int main() {
  int op, value;
  BinaryNode *root = NULL;

  do {
    printf("** Árvore Binária de Busca **\n");
    printf("1. Inserir\n");
    printf("2. Exibir\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &op);

    switch(op) {
      case 0:
        printf("Fim da execução\n");
        break;
      case 1:
        printf("Digite um valor: ");
        scanf(" %d", &value);
        root = insertBinaryNode(root, value);
        break;
      case 2:
        printf("Imprimindo árvore em ordem: \n");
        
    }
  } while (op != 0);
}

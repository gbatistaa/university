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
    printf(" %d", root->value);
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

// Function the sums all the leafs values in a binary search tree
int sumTreeLeafs(BinaryNode *root) {

  // Base case
  if (root == NULL)
    return 0;

  // Operations
  if (root->leftChild == NULL && root->rightChild == NULL)
    return root->value;
  else {
    int sumLeft = sumTreeLeafs(root->leftChild);
    int sumRight = sumTreeLeafs(root->rightChild);
    return sumLeft + sumRight;
  }
}

int main() {
  int op, value;
  BinaryNode *root = NULL;

  do {
    printf("\n");
    printf("\n** Árvore Binária de Busca **\n");
    printf("1. Inserir\n");
    printf("2. Exibir\n");
    printf("3. Exibir soma de todas as folhas\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &op);

    switch(op) {
      case 0:
        printf("\nFim da execução\n");
        break;
      case 1:
        printf("\nDigite um valor: ");
        scanf(" %d", &value);
        root = insertBinaryNode(root, value);
        break;
      case 2:
        printf("\nImprimindo árvore em ordem: \n");
        if(root == NULL)
          printf("Árvore sem elementos.\n");
        else
          printInOrder(root);
        break;
      case 3:
        int leafsTotalSum = sumTreeLeafs(root);
        printf("\nSoma de todas as folhas: %d\n", leafsTotalSum);
        break;
      default:
        printf("Opção Inválida!\n");
    }
  } while (op != 0);
}

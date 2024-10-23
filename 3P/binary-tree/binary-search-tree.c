#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
  int value;
  struct node *leftChild;
  struct node *rightChild;
} BinaryNode;

BinaryNode *createBinaryNode(int num)
{
  BinaryNode *newNode = (BinaryNode *)malloc(sizeof(BinaryNode));
  if (newNode == NULL)
  {
    printf("Erro na alocação de memória");
    return NULL;
  }

  newNode->value = num;
  newNode->rightChild = NULL;
  newNode->leftChild = NULL;

  return newNode;
}

int printInOrder(BinaryNode *rootNode)
{
  if (rootNode != NULL)
  {
    printInOrder(rootNode->leftChild);
    printf("%d\n", rootNode->value);
    printInOrder(rootNode->rightChild);

    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

BinaryNode *searchNode(BinaryNode *rootNode, int value)
{
  if (rootNode == NULL || value == rootNode->value)
    return rootNode;
  else if (value < rootNode->value)
    return searchNode(rootNode->leftChild, value);
  else
    return searchNode(rootNode->rightChild, value);
}

BinaryNode *insertBinaryNode(BinaryNode *rootNode, int value)
{
  if (rootNode == NULL)
  {
    BinaryNode *newNode = createBinaryNode(value);
    return newNode; // returns the new value;
  }

  if (rootNode->value == value)
    return rootNode;
  if (value < rootNode->value)
    rootNode->leftChild = insertBinaryNode(rootNode->leftChild, value);
  else
    rootNode->rightChild = insertBinaryNode(rootNode->rightChild, value);

  return rootNode;
}

BinaryNode *findSmallestNumber(BinaryNode *node)
{
  if (node->leftChild == NULL)
    return node;
  else
    return findSmallestNumber(node->leftChild);
}

BinaryNode *findSuccessor(BinaryNode *rootNode, int value)
{
  if (rootNode != NULL)
  {
    BinaryNode *currentSucessor = (searchNode(rootNode, value))->rightChild;
    if (currentSucessor != NULL) // condition triggered when the found element has an right child
    {
      BinaryNode *successor = findSmallestNumber(currentSucessor);
      return successor;
    }
  }
  return NULL;
}

BinaryNode **findNonNullChild(BinaryNode *parent)
{
  if (parent->leftChild != NULL)
    return &(parent->leftChild);

  else if (parent->rightChild != NULL)
    return &(parent->rightChild);
  return NULL;
}

// Removes the node with the passed value and return the root node of the BST
BinaryNode *removeBinaryNode(BinaryNode *rootNode, int value)
{
  if (rootNode == NULL) // Base case: empty tree or node not found
    return rootNode;

  // Step 1: Find the node to be removed
  if (value < rootNode->value)
    rootNode->leftChild = removeBinaryNode(rootNode->leftChild, value);
  else if (value > rootNode->value)
    rootNode->rightChild = removeBinaryNode(rootNode->rightChild, value);
  else
  {
    // Case 1: Node with one or no children
    if (rootNode->leftChild == NULL || rootNode->rightChild == NULL)
    {
      // The node to be removed is replaced by its non-null child
      BinaryNode *temp = rootNode->leftChild != NULL ? rootNode->leftChild : rootNode->rightChild;
      free(rootNode);
      return temp;
    }
    else
    {
      // Case 2: Node with two children

      // Find the successor (the smallest node in the right subtree)
      BinaryNode *successor = findSmallestNumber(rootNode->rightChild);

      // Copy the value of the successor to the current node
      rootNode->value = successor->value;

      // Remove the successor (repeated) from the right subtree
      rootNode->rightChild = removeBinaryNode(rootNode->rightChild, successor->value);
    }
  }
  return rootNode; // Return the updated pointer to the root
}

int main()
{
  BinaryNode *root = createBinaryNode(50);
  insertBinaryNode(root, 30);
  insertBinaryNode(root, 70);
  insertBinaryNode(root, 20);
  insertBinaryNode(root, 40);
  insertBinaryNode(root, 60);
  insertBinaryNode(root, 80);

  printInOrder(root);

  BinaryNode *node = searchNode(root, 70);
  BinaryNode *node2 = searchNode(root, 80);

  // removeBinaryNode(root, 20);
  // removeBinaryNode(root, 40);
  // removeBinaryNode(root, 80);

  BinaryNode *succ = findSuccessor(root, 70);
  printf("\n");
  printf("%p || %d\n", succ, succ->value);

  printf("\n");
  printInOrder(root);
  printf("\n");
  printf("\n");
  removeBinaryNode(root, 50);
  printInOrder(root);
  printf("\n");
  printf("\n");

  printf(node != NULL ? "Adress: %p\n" : "Not found\n", node);
  printf(node2 != NULL ? "Adress: %p\n" : "Not found\n", node2);

  return EXIT_SUCCESS;
}

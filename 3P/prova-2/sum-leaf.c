#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
  int value;
  struct node *leftChild;
  struct node *rightChild;
} BinaryNode;


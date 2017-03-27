#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct tree_node {
    char *type;
    char *content;
    struct tree_node *next;
    struct tree_node *child;
} Node;

Node* createNode(char *typeNode, char *contentNode, Node *relatedNode, char relation);
void printTree(Node *node, int depth);
void destroyTree(Node *node);

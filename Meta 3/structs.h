#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct tree_node {
    char * type;
    char * content;
    struct tree_node * next;
    struct tree_node * child;
    char * annotation;
} Node;

typedef struct symbol_table {
    char * type;
    char * name;
    char * params;
    char * flag;
    int flagMethod;
    struct symbol_table * next;
    struct symbol_table * child;
} Table;

Node* createNode(char *typeNode, char *contentNode, Node *relatedNode, char relation);
void printTree(Node *node, int depth);
void destroyTree(Node *node);

Table * createTableNode(char * type, char * name, char * params, char * flag, int flagMethod);
Table *semanticAnalysis(Node *tree);
void printTable(Table *symbolTable);

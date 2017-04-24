#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct tree_node {
    char * type;
    char * content;
    struct tree_node * next;
    struct tree_node * child;
    char * annotation;
} ASTNode;

typedef struct symbol_table {
    char * type;
    char * name;
    char * params;
    char * flag;
    int flagMethod;
    struct symbol_table * next;
    struct symbol_table * child;
} SymbolTableNode;

ASTNode* createASTNode(char *typeNode, char *contentNode, ASTNode *relatedNode, char relation);
void printTrees(ASTNode *node, int depth, int flagSymbomTable);
void destroyAST(ASTNode *node);

SymbolTableNode * createSymbolTableNode(char * type, char * name, char * params, char * flag, int flagMethod);
SymbolTableNode *semanticAnalysis(ASTNode *tree);
void printTable(SymbolTableNode *symbolTable);
void destroySymbolTable(SymbolTableNode *symbolTable);
void ASTSemanticAnnotations(ASTNode *tree, SymbolTableNode *symbolTable);

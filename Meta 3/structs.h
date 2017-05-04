#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct tree_node {
    char * type;
    char * content;
    int flagSkip;
    int line;
    int column;
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

typedef struct payload_node {
    char * value;
    int line;
    int column;
} Payload;

ASTNode* createASTNode(char * typeNode, Payload * content, ASTNode * relatedNode, char relation);
void printTrees(ASTNode * node, int depth, int flagSymbomTable);
void destroyAST(ASTNode * node);

SymbolTableNode * createSymbolTableNode(char * type, char * name, char * params, char * flag, int flagMethod);
SymbolTableNode * semanticAnalysis(ASTNode * tree);
void printTable(SymbolTableNode * symbolTable);
void destroySymbolTable(SymbolTableNode * symbolTable);
void ASTSemanticAnnotations(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable);
char * checkVariableExistance(ASTNode * astnode, SymbolTableNode * stnode, SymbolTableNode * currentMethodNode, int flagLocal, int flagGlobal);
char * getMethodType(ASTNode * astnode, SymbolTableNode * stnode);
int isSameMethod(SymbolTableNode * current, SymbolTableNode * method);
char * parseNumbers(char * number, int *zero);

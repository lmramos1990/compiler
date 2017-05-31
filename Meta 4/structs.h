#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct symbol_table {
    char * type;
    char * name;
    char * params;
    char * flag;
    int flagMethod;
    struct symbol_table * next;
    struct symbol_table * child;
} SymbolTableNode;

typedef struct tree_node {
    char * type;
    char * content;
    char * llvmCode;
    int flagSkip;
    int line;
    int column;
    struct tree_node * next;
    struct tree_node * child;
    char * annotation;
    SymbolTableNode * stnode;
} ASTNode;

typedef struct payload_node {
    char * value;
    int line;
    int column;
} Payload;

Payload * createPayloadNode(char *value, int line, int column);

ASTNode * createASTNode(char * typeNode, Payload * content, ASTNode * relatedNode, char relation);
void printTrees(ASTNode * node, int depth, int flagSymbomTable);
void destroyAST(ASTNode * node);

SymbolTableNode * createSymbolTableNode(char * type, char * name, char * params, char * flag, int flagMethod);
SymbolTableNode * semanticAnalysis(ASTNode * tree);
void printTable(SymbolTableNode * symbolTable);
void destroySymbolTable(SymbolTableNode * symbolTable);
void ASTSemanticAnnotations(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable);
SymbolTableNode * checkVariableExistance(ASTNode * astnode, SymbolTableNode * stnode, SymbolTableNode * currentMethodNode, int flagLocal, int flagGlobal);
char * getMethodType(ASTNode * astnode, SymbolTableNode * stnode);
int isSameMethod(SymbolTableNode * current, SymbolTableNode * method);
char * parseNumbers(char * number, int *zero);

void generateIntermidiateRepresentation(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable);
char * removeUnderscores(char * number);
void doStrings(ASTNode *node);
char * parseParameters(char * params);

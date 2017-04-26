#include "structs.h"

ASTNode* createASTNode(char *typeNode, char *contentNode, ASTNode *relatedNode, char relation) {
    ASTNode *newNode;

    newNode = (ASTNode*) malloc(sizeof(ASTNode));

    newNode->type = strdup(typeNode);
    if (contentNode != NULL)
        newNode->content = contentNode;
    else
        newNode->content = NULL;

    switch (relation) {
        case 'c':   /* child node */
            newNode->next = NULL;
            newNode->child = relatedNode;
            break;
        case 's':   /* sibling node */
            newNode->next = relatedNode;
            newNode->child = NULL;
            break;
        default:
            newNode->next = NULL;
            newNode->child = NULL;
            break;
    }

    return newNode;
}

void printTrees(ASTNode *node, int depth, int flagSymbomTable) {
    int i;

    if (node == NULL)
        return;

    for (i = 0; i < depth; i++) {
        printf("..");
    }
    printf("%s", node->type);
    if (node->content != NULL)
        printf("(%s)", node->content);

    if (flagSymbomTable)
        if (node->annotation != NULL)
            printf(" - %s", node->annotation);

    printf("\n");

    if (node->child != NULL)
        printTrees(node->child, depth + 1, flagSymbomTable);
    if (node->next != NULL)
        printTrees(node->next, depth, flagSymbomTable);
}

void destroyAST(ASTNode *node) {
    if (node == NULL)
        return;

    if (node->next != NULL)
        destroyAST(node->next);
    if (node->child != NULL)
        destroyAST(node->child);

    free(node->type);
    if (node->content != NULL)
        free(node->content);
    if (node->annotation != NULL)
        free(node->annotation);
    free(node);
}

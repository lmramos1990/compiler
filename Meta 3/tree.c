#include "structs.h"

Node* createNode(char *typeNode, char *contentNode, Node *relatedNode, char relation) {
    Node *newNode;

    newNode = (Node*) malloc(sizeof(Node));

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

void printTree(Node *node, int depth) {
    int i;

    if (node == NULL)
        return;

    for (i = 0; i < depth; i++) {
        printf("..");
    }
    printf("%s", node->type);
    if (node->content != NULL)
        printf("(%s)", node->content);
    printf("\n");

    if (node->child != NULL)
        printTree(node->child, depth + 1);
    if (node->next != NULL)
        printTree(node->next, depth);
}

void destroyTree(Node *node) {
    if (node == NULL)
        return;

    if (node->next != NULL)
        destroyTree(node->next);
    if (node->child != NULL)
        destroyTree(node->child);

    free(node->type);
    if (node->content != NULL)
        free(node->content);
    free(node);
}

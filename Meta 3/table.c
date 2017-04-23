#include "structs.h"

Table *createTableNode(char * type, char * name, char * params, char * flag, int flagMethod) {
    Table * newNode;

    newNode = (Table *) malloc(sizeof(Table));

    if (strcmp(type, "Class") == 0 || strcmp(type, "String[]") == 0)
        newNode->type = strdup(type);
    else if (type[0] >='A' && type[0] <= 'Z') {
        newNode->type = strdup(type);
        (newNode->type)[0] = (newNode->type)[0] + 'a' - 'A';
    } else
        newNode->type = strdup(type);

    newNode->name = strdup(name);

    if(params != NULL) {
        newNode->params = strdup(params);
    } else {
        newNode->params = NULL;
    }
    
    if(flag != NULL) {
        newNode->flag = flag;
    } else {
        newNode->flag = NULL;
    }

    newNode->flagMethod = flagMethod;

    return newNode;
}

Table *semanticAnalysis(Node *tree) {
    Node  *aux = tree, *auxMethod;
    Table *table, *auxTableNode, *root, *auxTableMethodNode;
    int flagClassCreated;
    char typeParams[1024];

    while(aux != NULL) {
        if (strcmp(aux->type, "Program") == 0) {
            table = createTableNode("Class", aux->child->content, NULL, NULL, 0);
            root = table;
            aux = aux->child->next;
            flagClassCreated = 1;
        } else if (strcmp(aux->type, "FieldDecl") == 0) {
            if (flagClassCreated) {
                table->child = createTableNode(aux->child->type, aux->child->next->content, NULL, NULL, 0);
                table = table->child;
                flagClassCreated = 0;
            } else {
                table->next = createTableNode(aux->child->type, aux->child->next->content, NULL, NULL, 0);
                table = table->next;
            }

            aux = aux->next;
        } else if (strcmp(aux->type, "MethodDecl") == 0) {
            auxMethod = aux->child->child; /* Enter MethodHeader */

            auxTableNode = createTableNode(auxMethod->type, auxMethod->next->content, NULL, NULL, 1);
            auxTableMethodNode = auxTableNode;
            if (flagClassCreated) {
                table->child = auxTableNode;
                table = table->child;
                flagClassCreated = 0;
            } else {
                table->next = auxTableNode;
                table = table->next;
            }
            auxTableNode->child = createTableNode(auxMethod->type, "return", NULL, NULL, 0);
            auxTableNode = auxTableNode->child;

            auxMethod = auxMethod->next->next;  /* MethodParams */
            if (auxMethod->child != NULL) {
                auxMethod = auxMethod->child;

                if (strcmp(auxMethod->child->type, "StringArray") == 0) {
                    sprintf(typeParams, "%s", "String[]");
                    auxTableNode->next = createTableNode(typeParams, auxMethod->child->next->content, NULL, "param", 0);
                    auxTableNode = auxTableNode->next;
                } else {
                    sprintf(typeParams, "%c%s", (auxMethod->child->type)[0] + 'a' - 'A', (auxMethod->child->type) + 1);
                    auxTableNode->next = createTableNode(typeParams, auxMethod->child->next->content, NULL, "param", 0);
                    auxTableNode = auxTableNode->next;
                }
                auxMethod = auxMethod->next;

                while (auxMethod != NULL) {
                    sprintf(typeParams, "%s,%c%s", typeParams, (auxMethod->child->type)[0] + 'a' - 'A', (auxMethod->child->type) + 1);
                    auxTableNode->next = createTableNode(auxMethod->child->type, auxMethod->child->next->content, NULL, "param", 0);
                    auxTableNode = auxTableNode->next;
                    auxMethod = auxMethod->next;
                }

                char newTypeParams[1024];
                sprintf(newTypeParams, "(%s)", typeParams);

                auxTableMethodNode->params = strdup(newTypeParams);
            }

            auxMethod = aux->child->next;   /* MethodBody */
            if (auxMethod->child != NULL) {
                auxMethod = auxMethod->child;

                while (auxMethod != NULL) {
                    if (strcmp(auxMethod->type, "VarDecl") == 0) {
                        auxTableNode->next = createTableNode(auxMethod->child->type, auxMethod->child->next->content, NULL, NULL, 0);
                        auxTableNode = auxTableNode->next;
                    }
                    auxMethod = auxMethod->next;
                }
            }

            aux = aux->next;
        }
    }

    return root;
}

void printTable(Table *symbolTable) {
    Table *aux = symbolTable, *aux2;

    if(symbolTable->child == NULL) {
        printf("Symbol Table Appears to be NULL\n");
        return;
    }

    while(aux != NULL) {
        if (strcmp(aux->type, "Class") == 0) {
            printf("===== Class %s Symbol Table =====\n", aux->name);
            aux = aux->child;
        } else {
            if(aux->flag == NULL) {
                if(aux->params == NULL) {
                    printf("%s\t\t%s\n", aux->name, aux->type);
                } else {
                    printf("%s\t%s\t%s\n", aux->name, aux->params, aux->type);
                }
            } else {
                if(aux->params == NULL) {
                    printf("%s\t\t%s\t%s\n", aux->name, aux->type, aux->flag);
                } else {
                    printf("%s\t%s\t%s\t%s\n", aux->name, aux->params, aux->type, aux->flag);
                }
            }
            aux = aux->next;
        }
    }

    aux = symbolTable->child;

    while (aux != NULL) {
        if (aux->flagMethod) {
            if(aux->params == NULL) {
                printf("\n===== Method %s Symbol Table =====\n", aux->name);
            } else {
                printf("\n===== Method %s%s Symbol Table =====\n", aux->name, aux->params);
            }

            aux2 = aux->child;
            while(aux2 != NULL) {
                if(aux2->flag == NULL) {
                    if(aux2->params == NULL) {
                        printf("%s\t\t%s\n", aux2->name, aux2->type);
                    } else {
                        printf("%s\t%s\t%s\n", aux2->name, aux2->params, aux2->type);
                    }
                } else {

                    if(aux2->params == NULL) {
                        printf("%s\t\t%s\t%s\n", aux2->name, aux2->type, aux2->flag);
                    } else {
                        printf("%s\t%s\t%s\t%s\n", aux2->name, aux2->params, aux2->type, aux2->flag);
                    }
                }

                aux2 = aux2->next;
            }
        }

        aux = aux->next;
    }

}

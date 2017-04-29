#include "structs.h"


SymbolTableNode *createSymbolTableNode(char * type, char * name, char * params, char * flag, int flagMethod) {
    SymbolTableNode * newNode;

    newNode = (SymbolTableNode*) malloc(sizeof(SymbolTableNode));

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
        newNode->flag = strdup(flag);
    } else {
        newNode->flag = NULL;
    }

    newNode->flagMethod = flagMethod;

    return newNode;
}

SymbolTableNode *semanticAnalysis(ASTNode *tree) {
    ASTNode  *aux = tree, *auxMethod;
    SymbolTableNode *table, *auxTableNode, *root, *auxTableMethodNode;
    int flagClassCreated;
    char typeParams[1024];

    // printf("LINE: %d COLUMN: %d\n", tree -> line, tree -> column);

    while (aux != NULL) {
        if (strcmp(aux->type, "Program") == 0) {
            table = createSymbolTableNode("Class", aux->child->content, NULL, NULL, 0);
            root = table;
            aux = aux->child->next;
            flagClassCreated = 1;
        } else if (strcmp(aux->type, "FieldDecl") == 0) {
            if (flagClassCreated) {
                if (strcmp(aux->child->type, "Bool") == 0)
                    table->child = createSymbolTableNode("boolean", aux->child->next->content, NULL, NULL, 0);
                else
                    table->child = createSymbolTableNode(aux->child->type, aux->child->next->content, NULL, NULL, 0);
                table = table->child;
                flagClassCreated = 0;
            } else {
                if (strcmp(aux->child->type, "Bool") == 0)
                    table->next = createSymbolTableNode("boolean", aux->child->next->content, NULL, NULL, 0);
                else
                    table->next = createSymbolTableNode(aux->child->type, aux->child->next->content, NULL, NULL, 0);
                table = table->next;
            }

            aux = aux->next;
        } else if (strcmp(aux->type, "MethodDecl") == 0) {
            auxMethod = aux->child->child; /* Enter MethodHeader */

            if (strcmp(auxMethod->type, "Bool") == 0)
                auxTableNode = createSymbolTableNode("boolean", auxMethod->next->content, NULL, NULL, 1);
            else
                auxTableNode = createSymbolTableNode(auxMethod->type, auxMethod->next->content, NULL, NULL, 1);
            auxTableMethodNode = auxTableNode;
            if (flagClassCreated) {
                table->child = auxTableNode;
                table = table->child;
                flagClassCreated = 0;
            } else {
                table->next = auxTableNode;
                table = table->next;
            }
            if (strcmp(auxMethod->type, "Bool") == 0)
                auxTableNode->child = createSymbolTableNode("boolean", "return", NULL, NULL, 0);
            else
                auxTableNode->child = createSymbolTableNode(auxMethod->type, "return", NULL, NULL, 0);
            auxTableNode = auxTableNode->child;

            auxMethod = auxMethod->next->next;  /* MethodParams */
            if (auxMethod->child != NULL) {
                auxMethod = auxMethod->child;

                if (strcmp(auxMethod->child->type, "StringArray") == 0) {
                    sprintf(typeParams, "%s", "String[]");
                    auxTableNode->next = createSymbolTableNode(typeParams, auxMethod->child->next->content, NULL, "param", 0);
                    auxTableNode = auxTableNode->next;
                } else if (strcmp(auxMethod->child->type, "Bool") == 0) {
                    sprintf(typeParams, "%s", "boolean");
                    auxTableNode->next = createSymbolTableNode(typeParams, auxMethod->child->next->content, NULL, "param", 0);
                    auxTableNode = auxTableNode->next;
                } else {
                    sprintf(typeParams, "%c%s", (auxMethod->child->type)[0] + 'a' - 'A', (auxMethod->child->type) + 1);
                    auxTableNode->next = createSymbolTableNode(typeParams, auxMethod->child->next->content, NULL, "param", 0);
                    auxTableNode = auxTableNode->next;
                }
                auxMethod = auxMethod->next;

                while (auxMethod != NULL) {
                    if (strcmp(auxMethod->child->type, "Bool") == 0) {
                        sprintf(typeParams, "%s,%s", typeParams, "boolean");
                        auxTableNode->next = createSymbolTableNode("boolean", auxMethod->child->next->content, NULL, "param", 0);
                    } else {
                        sprintf(typeParams, "%s,%c%s", typeParams, (auxMethod->child->type)[0] + 'a' - 'A', (auxMethod->child->type) + 1);
                        auxTableNode->next = createSymbolTableNode(auxMethod->child->type, auxMethod->child->next->content, NULL, "param", 0);
                    }
                    auxTableNode = auxTableNode->next;
                    auxMethod = auxMethod->next;
                }

                char newTypeParams[1024];
                sprintf(newTypeParams, "(%s)", typeParams);

                auxTableMethodNode->params = strdup(newTypeParams);
            } else {
                auxTableMethodNode->params = strdup("()");
            }

            auxMethod = aux->child->next;   /* MethodBody */
            if (auxMethod->child != NULL) {
                auxMethod = auxMethod->child;

                while (auxMethod != NULL) {
                    if (strcmp(auxMethod->type, "VarDecl") == 0) {
                        if (strcmp(auxMethod->child->type, "Bool") == 0)
                            auxTableNode->next = createSymbolTableNode("boolean", auxMethod->child->next->content, NULL, NULL, 0);
                        else
                            auxTableNode->next = createSymbolTableNode(auxMethod->child->type, auxMethod->child->next->content, NULL, NULL, 0);
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

void printTable(SymbolTableNode *symbolTable) {
    SymbolTableNode *aux = symbolTable, *aux2;

    if (symbolTable->child == NULL) {
        printf("===== Class %s Symbol Table =====\n", aux->name);
        return;
    }

    while (aux != NULL) {
        if (strcmp(aux->type, "Class") == 0) {
            printf("===== Class %s Symbol Table =====\n", aux->name);
            aux = aux->child;
        } else {
            if (aux->flag == NULL) {
                if (aux->params == NULL) {
                    printf("%s\t\t%s\n", aux->name, aux->type);
                } else {
                    printf("%s\t%s\t%s\n", aux->name, aux->params, aux->type);
                }
            } else {
                if (aux->params == NULL) {
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
            printf("\n===== Method %s%s Symbol Table =====\n", aux->name, aux->params);

            aux2 = aux->child;
            while (aux2 != NULL) {
                if (aux2->flag == NULL) {
                    if (aux2->params == NULL) {
                        printf("%s\t\t%s\n", aux2->name, aux2->type);
                    } else {
                        printf("%s\t%s\t%s\n", aux2->name, aux2->params, aux2->type);
                    }
                } else {
                    if (aux2->params == NULL) {
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

void destroySymbolTable(SymbolTableNode *symbolTable) {
    if (symbolTable == NULL)
        return;

    free(symbolTable->type);
    free(symbolTable->name);
    if (symbolTable->params != NULL)
        free(symbolTable->params);
    if (symbolTable->flag != NULL)
        free(symbolTable->flag);

    destroySymbolTable(symbolTable->child);
    destroySymbolTable(symbolTable->next);
}

void ASTSemanticAnnotations(ASTNode *node, SymbolTableNode *symbolTable, SymbolTableNode *currentMethodNode, int flagVariable) {
    ASTNode *child1, *child2;
    SymbolTableNode *auxSymbolTable;
    char *varType;

    if (node == NULL)
        return;

    /* ver em que metodo estamos para fazer verificação de vars e guarda ponteiro do no da
        symbol table para passar por argumento na recusao para nao se ter de estar sempre
        a fazer o ciclo while?
    */

    if (flagVariable) {
        if (strcmp(node->type, "Id") == 0) {
            varType = checkVariableExistance(node, symbolTable, currentMethodNode);
            if (varType != NULL) {
                node->annotation = strdup(varType);
            } else {
                printf("---> ERROR ID <---------------------------------------\n");
            }
            return;
        } else if (strcmp(node->type, "DecLit") == 0) {
            node->annotation = strdup("int");
            return;
        } else if (strcmp(node->type, "RealLit") == 0) {
            node->annotation = strdup("double");
            return;
        } else if (strcmp(node->type, "BoolLit") == 0) {
            node->annotation = strdup("boolean");
            return;
        } else if (strcmp(node->type, "StrLit") == 0) {
            node->annotation = strdup("String");
            return;
        }
    }

    if (strcmp(node->type, "Assign") == 0) {
        child1 = node->child;
        child2 = node->child->next;

        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);
        ASTSemanticAnnotations(child2, symbolTable, currentMethodNode, 1);

        if (strcmp(child1->annotation, child2->annotation) == 0) {
            node->annotation = strdup(child1->annotation);
        } else {
            printf("---> ERROR ASSIGN <---------------------------------------\n"); // error + double = int + etc
        }
    } else if (strcmp(node->type, "Call") == 0) {
        child1 = node->child;
        char * methodType = getMethodType(child1, symbolTable, currentMethodNode);
        node->annotation = strdup(methodType);

    } else if (strcmp(node->type, "ParseArgs") == 0) {
        child1 = node->child;
        child2 = node->child->next;

        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);

        if (child1->annotation != NULL) {
            if (strcmp(child1->annotation, "String[]") == 0) {
                ASTSemanticAnnotations(child2, symbolTable, currentMethodNode, 1);

                if (child2->annotation != NULL) {
                    if (strcmp(child2->annotation, "int") == 0) {
                        ;
                    } else {
                        // erro
                        ;
                    }
                } else {

                }
            } else {
                //error
                ;
            }

        } else {
            // error
            ;
        }

        node->annotation = strdup("int");
    } else if (strcmp(node->type, "And") == 0 || strcmp(node->type, "Or") == 0 || strcmp(node->type, "Eq") == 0 || strcmp(node->type, "Geq") == 0 || strcmp(node->type, "Gt") == 0 || strcmp(node->type, "Leq") == 0 || strcmp(node->type, "Lt") == 0 || strcmp(node->type, "Neq") == 0) {
        child1 = node->child;
        child2 = node->child->next;

        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);
        ASTSemanticAnnotations(child2, symbolTable, currentMethodNode, 1);

        if (child1->annotation != NULL) {
            if (child2->annotation != NULL) {
                if (strcmp(child1->annotation, child2->annotation) == 0) {
                    ;
                } else {
                    // error
                    ;
                }
            } else {
                //error
                ;
            }
        } else {
            // error
            printf("---> ERROR AND|OR|EQ|GEQ|GT|LT|NEQ <---------------------------------------\n"); // error + double = int + etc
        }

        node->annotation = strdup("boolean");

    } else if (strcmp(node->type, "Add") == 0 || strcmp(node->type, "Sub") == 0 || strcmp(node->type, "Mul") == 0 || strcmp(node->type, "Div") == 0 || strcmp(node->type, "Mod") == 0) {
       child1 = node->child;
       child2 = node->child->next;

       ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);
       ASTSemanticAnnotations(child2, symbolTable, currentMethodNode, 1);

       if (strcmp(child1->annotation, child2->annotation) == 0) {
           node->annotation = strdup(child1->annotation);
       } else {
           printf("---> ERROR ADD|SUB|MUL|DIV|MOD|PLUS <---------------------------------------\n"); // error + double = int + etc
       }
    } else if (strcmp(node->type, "Minus") == 0 || strcmp(node->type, "Plus") == 0) {
        child1 = node->child;

        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);

        if (child1->annotation != NULL) {
            if (strcmp(child1->annotation, "int") == 0 || strcmp(child1->annotation, "double") == 0)
                node->annotation = strdup(child1->annotation);
        } else {
            // error
        }
    } else if (strcmp(node->type, "Not") == 0) {
        child1 = node->child;
        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);

        if (child1->annotation != NULL) {
            if (strcmp(child1->annotation, "boolean") == 0) {
                node->annotation = strdup("boolean");
            } else {
                //error
                ;
            }
        } else {
            //error
            ;
        }
    } else if (strcmp(node->type, "Print") == 0) {
        child1 = node->child;
        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);

        if (child1->annotation == NULL) {
            // error
        }

    } else if (strcmp(node->type, "Length") == 0) {
        child1 = node->child;
        
        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);

        if(child1->annotation == NULL) {
            // error
            ;
        } else if (strcmp(child1->type, "String[]") == 0) {
            ;
        } else {
            // error
            ;
        }

        node->annotation = strdup("int");

    } else if (strcmp(node->type, "Return") == 0) {
        child1 = node->child;

        ASTSemanticAnnotations(child1, symbolTable, currentMethodNode, 1);

        if(child1->annotation != NULL) {
            // comparar
            if(strcmp(currentMethodNode->type, child1->annotation) != 0) {
                // error
                ;
            }
        } else {
            // error
            ;
        }

        //method to chekc if the child type is the same as the return type of the method
    } else if (strcmp(node->type, "MethodHeader") == 0) {
        auxSymbolTable = symbolTable->child;

        while (auxSymbolTable != NULL) {
            if (auxSymbolTable->flagMethod && strcmp(auxSymbolTable->name, node->child->next->content) == 0) {
                currentMethodNode = auxSymbolTable;
                break;
            }
            auxSymbolTable = auxSymbolTable->next;
        }
    } else {
        ASTSemanticAnnotations(node->child, symbolTable, currentMethodNode, 0);
    }

    ASTSemanticAnnotations(node->next, symbolTable, currentMethodNode, 0);
}

char * checkVariableExistance(ASTNode * astnode, SymbolTableNode * stnode, SymbolTableNode * currentMethodNode) {
    char *variableName;
    SymbolTableNode *aux;

    if(stnode == NULL || currentMethodNode == NULL) {
        return NULL;
    }

    if(astnode->content != NULL) {
        variableName = astnode->content;
    }

    // check local variables
    aux = currentMethodNode->child;

    while(aux != NULL) {
        if(strcmp(aux->name, variableName) == 0) {
            return aux->type;
        }

        aux = aux->next;
    }

    // check global variables
    aux = stnode->child;

    while(aux != NULL) {
        if(!(aux->flagMethod) && strcmp(aux->name, variableName) == 0) {
            return aux->type;
        }

        aux = aux->next;
    }

    return NULL;
}

char * getMethodType(ASTNode * astnode, SymbolTableNode * stnode, SymbolTableNode * currentMethodNode) {
    SymbolTableNode * aux, * aux2;

    if(stnode == NULL) {
        return NULL;
    }

    aux = stnode->child;

    while(aux != NULL) {
        if(aux->flagMethod && strcmp(astnode->content, aux->name) == 0) {
            astnode->annotation = strdup(aux->params);

            aux2 = aux->child->next;
            astnode = astnode->next;

            while(astnode != NULL && aux2 != NULL) {
                ASTSemanticAnnotations(astnode, stnode, currentMethodNode, 1);

                if(astnode->annotation == NULL) {
                    printf("ERROR: ID NOT EXISTENT\n");
                    break;
                }

                if(strcmp(aux2->type, astnode->annotation) != 0) {
                    // ERROR
                    break;
                }

                aux2 = aux2->next;
                astnode = astnode->next;
            }

            if ((astnode == NULL && aux2 != NULL) || (astnode != NULL && aux2 == NULL)) {
                // error
                ;
            }

            return aux->type;
        }

        aux = aux->next;
    }

    return NULL;
}

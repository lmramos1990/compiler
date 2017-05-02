#include "structs.h"

SymbolTableNode * AnnotationsCurrentMethodNode = NULL;

SymbolTableNode * createSymbolTableNode(char * type, char * name, char * params, char * flag, int flagMethod) {
    SymbolTableNode * newNode;

    newNode = (SymbolTableNode *) malloc(sizeof(SymbolTableNode));

    if(strcmp(type, "Class") == 0 || strcmp(type, "String[]") == 0)
        newNode -> type = strdup(type);
    else if(type[0] >='A' && type[0] <= 'Z') {
        newNode -> type = strdup(type);
        (newNode -> type)[0] = (newNode -> type)[0] + 'a' - 'A';
    } else
        newNode -> type = strdup(type);

    newNode -> name = strdup(name);

    if(params != NULL) {
        newNode -> params = strdup(params);
    } else {
        newNode -> params = NULL;
    }

    if(flag != NULL) {
        newNode -> flag = strdup(flag);
    } else {
        newNode -> flag = NULL;
    }

    newNode -> flagMethod = flagMethod;

    newNode->child = NULL;
    newNode->next = NULL;

    return newNode;
}

SymbolTableNode * semanticAnalysis(ASTNode * tree) {
    ASTNode * aux = tree, * auxMethod, * currentMethod;
    SymbolTableNode * table, * auxTableNode, * root, * auxTableMethodNode, * tablePrevious;
    int flagClassCreated;
    char typeParams[1024];

    while(aux != NULL) {
        if(strcmp(aux -> type, "Program") == 0) {
            table = createSymbolTableNode("Class", aux -> child -> content, NULL, NULL, 0);
            root = table;
            aux = aux -> child -> next;
            flagClassCreated = 1;
        } else if(strcmp(aux -> type, "FieldDecl") == 0) {
            if(flagClassCreated) {
                if(strcmp(aux -> child -> type, "Bool") == 0) {
                    table -> child = createSymbolTableNode("boolean", aux -> child -> next -> content, NULL, NULL, 0);
                } else {
                    table -> child = createSymbolTableNode(aux->child->type, aux->child->next->content, NULL, NULL, 0);
                }

                table = table->child;
                flagClassCreated = 0;
            } else {
                if(strcmp(aux->child->type, "Bool") == 0) {
                    table->next = createSymbolTableNode("boolean", aux->child->next->content, NULL, NULL, 0);
                } else {
                    table->next = createSymbolTableNode(aux->child->type, aux->child->next->content, NULL, NULL, 0);
                }

                table = table->next;
            }

            aux = aux->next;
        } else if (strcmp(aux->type, "MethodDecl") == 0) {
            auxMethod = aux->child->child; /* Enter MethodHeader */

            currentMethod = auxMethod -> next;

            if (strcmp(auxMethod->type, "Bool") == 0)
                auxTableNode = createSymbolTableNode("boolean", auxMethod->next->content, NULL, NULL, 1);
            else
                auxTableNode = createSymbolTableNode(auxMethod->type, auxMethod->next->content, NULL, NULL, 1);
            auxTableMethodNode = auxTableNode;
            tablePrevious = table;
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
                    if(checkVariableExistance(auxMethod -> child -> next, auxTableMethodNode, auxTableMethodNode, 1) != NULL) {
                        printf("Line %d, col %d: Symbol %s already defined\n", auxMethod -> child -> next -> line, auxMethod -> child -> next -> column, auxMethod -> child -> next -> content);
                    }

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

            SymbolTableNode * stnaux = root -> child;
            int sameMethod = 0;
            while(stnaux != NULL) {
                if(stnaux -> flagMethod && stnaux != auxTableMethodNode) {
                    if(isSameMethod(stnaux, auxTableMethodNode)) {
                        printf("Line %d, col %d: Symbol %s%s already defined\n", currentMethod -> line, currentMethod -> column, currentMethod -> content, auxTableMethodNode -> params);
                        destroySymbolTable(auxTableMethodNode);
                        sameMethod = 1;
                        break;
                    }
                }

                stnaux = stnaux -> next;
            }

            if(sameMethod) {
                table = tablePrevious;
                table -> next = NULL;
                aux = aux -> next;
                continue;
            }

            auxMethod = aux -> child -> next;   /* MethodBody */
            if(auxMethod -> child != NULL) {
                auxMethod = auxMethod -> child;

                while(auxMethod != NULL) {
                    if(strcmp(auxMethod -> type, "VarDecl") == 0) {

                        if(checkVariableExistance(auxMethod -> child -> next, root, auxTableMethodNode, 0) != NULL) {
                            printf("Line %d, col %d: Symbol %s already defined\n", auxMethod -> child -> next -> line, auxMethod -> child -> next -> column, auxMethod -> child -> next -> content);
                        }

                        if(strcmp(auxMethod -> child -> type, "Bool") == 0) {
                            auxTableNode -> next = createSymbolTableNode("boolean", auxMethod -> child -> next -> content, NULL, NULL, 0);
                        } else {
                            auxTableNode -> next = createSymbolTableNode(auxMethod -> child -> type, auxMethod -> child -> next -> content, NULL, NULL, 0);
                        }

                        auxTableNode = auxTableNode -> next;
                    }

                    auxMethod = auxMethod -> next;
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

void ASTSemanticAnnotations(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable) {
    ASTNode * child1, * child2;
    SymbolTableNode * auxSymbolTable;
    char * varType;

    if(node == NULL) {
        return;
    }

    if(flagVariable) {
        if(strcmp(node -> type, "Id") == 0) {
            varType = checkVariableExistance(node, symbolTable, AnnotationsCurrentMethodNode, 0);

            if(varType != NULL) {
                node -> annotation = strdup(varType);
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Cannot find symbol %s\n", node -> line, node -> column, node -> content);
            }
            return;
        } else if(strcmp(node -> type, "DecLit") == 0) {

            char * number = parseNumbers(node -> content);

            if(strlen(number) > 10 || atol(number) >= 2147483648) {
                printf("Line %d, col %d: Number %s out of bounds\n", node -> line, node -> column, node -> content);
            }
            free(number);
            node -> annotation = strdup("int");
            return;
        } else if(strcmp(node -> type, "RealLit") == 0) {
            node -> annotation = strdup("double");
            return;
        } else if(strcmp(node -> type, "BoolLit") == 0) {
            node -> annotation = strdup("boolean");
            return;
        } else if(strcmp(node -> type, "StrLit") == 0) {
            node -> annotation = strdup("String");
            return;
        }
    }

    if(strcmp(node -> type, "Assign") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(child2, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0 && strcmp(child2 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, child2 -> annotation) == 0) {
                node -> annotation = strdup(child1 -> annotation);
            } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {
                node -> annotation = strdup("double");
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", node -> child -> line, node -> child -> column, node -> type, child1 -> annotation, child2 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", node -> child -> line, node -> child -> column, node -> type, child1 -> annotation, child2 -> annotation);
        }
    } else if(strcmp(node -> type, "Call") == 0) {
        child1 = node -> child;
        ASTNode * params = child1 -> next;

        while(params != NULL) {
            ASTSemanticAnnotations(params, symbolTable, 1);
            params = params -> next;
        }

        char * methodType = getMethodType(child1, symbolTable);

        if(methodType == NULL) {
            printf("METHOD TYPE: NULL\n");
            node -> annotation = strdup("undef");
        } else {
            printf("METHOD TYPE: %s\n", methodType);
            node -> annotation = strdup(methodType);
        }

    } else if(strcmp(node -> type, "ParseArgs") == 0) {
        child1 = node->child;
        child2 = node->child->next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(child2, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0 && strcmp(child2 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, "String[]") == 0) {
                if(strcmp(child2 -> annotation, "int") == 0) {
                    node -> annotation = strdup("int");
                } else {
                    node -> annotation = strdup("undef");
                    printf("Line %d, col %d: Incompatible type %s in %s statement\n", node -> line, node -> column, child2 -> annotation, node -> type);
                }
            } else {
                node -> annotation = strdup("undef");
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Incompatible type undef in %s statement\n", node -> line, node -> column, node -> type);
        }
    } else if(strcmp(node -> type, "And") == 0 || strcmp(node -> type, "Or") == 0 || strcmp(node -> type, "Eq") == 0 || strcmp(node -> type, "Geq") == 0 || strcmp(node -> type, "Gt") == 0 || strcmp(node -> type, "Leq") == 0 || strcmp(node -> type, "Lt") == 0 || strcmp(node -> type, "Neq") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(child2, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0 && strcmp(child2 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, child2 -> annotation) == 0) {
                node -> annotation = strdup("boolean");
            } else if((strcmp(child1 -> annotation, "int") == 0 || strcmp(child1 -> annotation, "double") == 0) && (strcmp(child2 -> annotation, "int") == 0 || strcmp(child2 -> annotation, "double") == 0)) {
                node -> annotation = strdup("boolean");
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", node -> line, node -> column, node -> type, child1 -> annotation, child2 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
        }
    } else if (strcmp(node->type, "Add") == 0 || strcmp(node->type, "Sub") == 0 || strcmp(node->type, "Mul") == 0 || strcmp(node->type, "Div") == 0 || strcmp(node->type, "Mod") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(child2, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0 && strcmp(child2 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, child2 -> annotation) == 0) {
                node -> annotation = strdup(child1 -> annotation);
            } else if((strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) || (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0)) {
                node -> annotation = strdup("double");
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", node -> line, node -> column, node -> type, child1 -> annotation, child2 -> annotation);
        }
    } else if(strcmp(node -> type, "Minus") == 0 || strcmp(node -> type, "Plus") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, "int") == 0 || strcmp(child1 -> annotation, "double") == 0) {
                node -> annotation = strdup(child1 -> annotation);
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", node -> line, node -> column, node -> type, child1 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", node -> line, node -> column, node -> type, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "Not") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, "boolean") == 0) {
                node -> annotation = strdup("boolean");
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", node -> line, node -> column, node -> type, child1 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", node -> line, node -> column, node -> type, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "Print") == 0) {
        child1 = node -> child;
        ASTSemanticAnnotations(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") == 0) {
            printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", node -> line, node -> column, node -> type, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "Length") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, "String[]") == 0) {
                node -> annotation = strdup("int");
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", node -> line, node -> column, node -> type, child1 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", node -> line, node -> column, node -> type, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "Return") == 0) {

        child1 = node -> child;

        if(child1 == NULL) {
            if(strcmp(AnnotationsCurrentMethodNode -> type, "void") != 0) {
                printf("Line %d, col %d: Incompatible type void in %s statement\n", node -> line, node -> column, node -> type);
            }
        } else {
            ASTSemanticAnnotations(child1, symbolTable, 1);

            if(strcmp(child1 -> annotation, "undef") != 0) {
                if(strcmp(AnnotationsCurrentMethodNode -> type, child1 -> annotation) != 0) {
                    printf("Line %d, col %d: Incompatible type %s in %s statement\n", node -> line, node -> column, child1 -> annotation, node -> type);
                }
            } else {
                printf("Line %d, col %d: Incompatible type undef in %s statement\n", node -> line, node -> column, node -> type);
            }
        }

    } else if(strcmp(node -> type, "MethodHeader") == 0) {
        if (AnnotationsCurrentMethodNode == NULL)
            auxSymbolTable = symbolTable->child;
        else {
            auxSymbolTable = AnnotationsCurrentMethodNode->next;
        }

        while(auxSymbolTable != NULL) {
            if(auxSymbolTable -> flagMethod) {
                AnnotationsCurrentMethodNode = auxSymbolTable;
                break;
            }
            auxSymbolTable = auxSymbolTable -> next;
        }
    } else {
        ASTSemanticAnnotations(node->child, symbolTable, 0);
    }

    ASTSemanticAnnotations(node->next, symbolTable, 0);
}

char * checkVariableExistance(ASTNode * astnode, SymbolTableNode * stnode, SymbolTableNode * currentMethodNode, int flagLocal) {
    char *variableName;
    SymbolTableNode *aux;

    if(stnode == NULL || currentMethodNode == NULL) {
        return NULL;
    }

    if(astnode->content != NULL) {
        variableName = astnode->content;
    } else {
        return NULL;
    }

    // check local variables
    aux = currentMethodNode->child->next;

    while(aux != NULL) {
        if(strcmp(aux->name, variableName) == 0) {
            return aux->type;
        }

        aux = aux->next;
    }

    // check global variables
    if(!flagLocal) {
        aux = stnode->child;
        while(aux != NULL) {
            if(!(aux->flagMethod) && strcmp(aux->name, variableName) == 0) {
                return aux->type;
            }

            aux = aux->next;
        }
    }

    return NULL;
}

char * getMethodType(ASTNode * astnode, SymbolTableNode * stnode) {
    SymbolTableNode * aux, * aux2;
    int flagTaTudoBem, flagPerfectFit;

    if(stnode == NULL) {
        return NULL;
    }

    aux = stnode -> child;

    // same  name and same parameters!
    // same name different parameter continues
    // different names continues
    // int flagAmbiguity = 0;

    SymbolTableNode * lastFit = NULL;

    while(aux != NULL) {
        if(aux -> flagMethod && strcmp(astnode -> content, aux -> name) == 0) {
            // printf("ESTA E A FUNCAO QUE ESTOU A TESTAR: %s %s : FUNCAO DA TABELA\n", astnode -> content, aux -> name);
            ASTNode * params = astnode -> next;
            aux2 = aux -> child -> next;

            flagTaTudoBem = 1;
            flagPerfectFit = 1;
            while(params != NULL && aux2 != NULL) {
                // printf("TEST: %s %s : TABLE\n", params -> annotation, aux2 -> type);
                if(strcmp(params -> annotation, aux2 -> type) == 0) {
                    // printf("PARAMS HAVE THE SAME TYPE -> CONTINUE WITHOUT PROBLEMS\n");
                } else if(strcmp(params -> annotation, "int") == 0 && strcmp(aux2 -> type, "double") == 0) {
                    flagPerfectFit = 0;
                    // printf("PARAMS DONT HAVE THE SAME TYPE -> ACCEPTABLE\n");
                } else {
                    // printf("PARAMS DONT HAVE THE SAME TYPE -> UNACCEPTABLE\n");
                    flagTaTudoBem = 0;
                    break;
                }

                params = params -> next;
                aux2 = aux2 -> next;
            }

            if(!flagTaTudoBem) {
                aux = aux -> next;
                continue;
            }

            if(params == NULL && aux2 == NULL) {
                if(flagPerfectFit) {
                    astnode -> annotation = strdup(aux -> params);
                    return aux -> type;
                }

                if(lastFit == NULL) {
                    lastFit = aux;
                } else {
                    ; // ambiguo?
                }
/*
                // printf("last fit\n");
                //
                // if(lastFit == NULL) {
                //     lastFit = aux2;
                //     printf("last fit 2\n");
                // } else {
                //     flagAmbiguity = 1;
                //
                //     printf("hello\n");
                //     // se este for uma fit perfeita, isto é, os tipos do parametros sao todos exatamente iguais aos passados
                //     // entao devolver este no
                //     // senao "dizer" que é ambiguo? - visto haver mais do que um possivel mas nao best fit
                //     //     desde que para a frente nao haja um best fit
                //     //      basicamente por uma flag a 1 e no final verificar se nao houve best fit e flag a 1 dar ambiguidade
                // }
                if(lastFit == NULL) {
                    lastFit = aux2;
                }

                if(flagPerfectFit) {
                    printf("havia um fit que nao era perfeito mas acabei de encontrar um que e perfeito\n");
                } else if(!flagPerfectFit) {
                    printf("havia um fit que nao era perfeito e acabei de encontar outro que nao e perfeito\n");
                } else {
                    printf("vim parar ao else?\n");
                }
                */
            }
        }
        aux = aux -> next;
    }

    if (lastFit != NULL) {
        astnode -> annotation = strdup(lastFit -> params);
        return lastFit -> type;
    } else {
        astnode -> annotation = strdup("undef");
        return NULL;
    }
}

int isSameMethod(SymbolTableNode * current, SymbolTableNode * method) {
    int sameName = 0, sameParams = 0;
    if(strcmp(current -> name, method -> name) == 0) {
        sameName = 1;
    }

    if(strcmp(current -> params, method -> params) == 0) {
        sameParams = 1;
    }

    if(sameName && sameParams) {
        return 1;
    } else {
        return 0;
    }
}

char * parseNumbers(char * number) {
    int i = 0;
    int j = 0;
    char newNumber[1000];

    for(i = 0; i < strlen(number); i++) {
        if(number[i] != '_') {
            newNumber[j] = number[i];
            j++;
        }
    }

    newNumber[j] = '\0';

    return strdup(newNumber);
}

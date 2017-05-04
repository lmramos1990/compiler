#include "structs.h"

SymbolTableNode * AnnotationscurrentMethodNode = NULL;

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

    newNode -> child = NULL;
    newNode -> next = NULL;

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
                    table -> child = createSymbolTableNode(aux -> child -> type, aux -> child -> next -> content, NULL, NULL, 0);
                }

                table = table-> child;
                flagClassCreated = 0;
            } else {
                if(checkVariableExistance(aux -> child -> next, root, NULL, 0, 1) != NULL) {
                    printf("Line %d, col %d: Symbol %s already defined\n", aux -> child -> next -> line, aux -> child -> next -> column, aux -> child -> next -> content);
                    aux = aux -> next;
                    continue;
                } else if(strcmp(aux -> child -> type, "Bool") == 0) {
                    table -> next = createSymbolTableNode("boolean", aux -> child -> next -> content, NULL, NULL, 0);
                } else {
                    table->next = createSymbolTableNode(aux -> child->type, aux -> child -> next -> content, NULL, NULL, 0);
                }

                table = table->next;
            }

            aux = aux -> next;
        } else if(strcmp(aux -> type, "MethodDecl") == 0) {
            auxMethod = aux -> child -> child; /* Enter MethodHeader */

            currentMethod = auxMethod -> next;

            if(strcmp(auxMethod->type, "Bool") == 0) {
                auxTableNode = createSymbolTableNode("boolean", auxMethod->next-> content, NULL, NULL, 1);
            } else {
                auxTableNode = createSymbolTableNode(auxMethod->type, auxMethod->next-> content, NULL, NULL, 1);
            }
            auxTableMethodNode = auxTableNode;
            tablePrevious = table;
            if(flagClassCreated) {
                table -> child = auxTableNode;
                table = table -> child;
                flagClassCreated = 0;
            } else {
                table -> next = auxTableNode;
                table = table -> next;
            }
            if(strcmp(auxMethod -> type, "Bool") == 0) {
                auxTableNode -> child = createSymbolTableNode("boolean", "return", NULL, NULL, 0);
            } else {
                auxTableNode -> child = createSymbolTableNode(auxMethod->type, "return", NULL, NULL, 0);
            }
            auxTableNode = auxTableNode -> child;

            auxMethod = auxMethod->next->next;  /* MethodParams */
            if(auxMethod-> child != NULL) {
                auxMethod = auxMethod-> child;

                if(strcmp(auxMethod-> child->type, "StringArray") == 0) {
                    sprintf(typeParams, "%s", "String[]");
                    auxTableNode -> next = createSymbolTableNode("String[]", auxMethod-> child->next-> content, NULL, "param", 0);
                    auxTableNode = auxTableNode -> next;
                } else if(strcmp(auxMethod-> child->type, "Bool") == 0) {
                    sprintf(typeParams, "%s", "boolean");
                    auxTableNode -> next = createSymbolTableNode("boolean", auxMethod-> child->next-> content, NULL, "param", 0);
                    auxTableNode = auxTableNode -> next;
                } else {
                    sprintf(typeParams, "%c%s", (auxMethod-> child->type)[0] + 'a' - 'A', (auxMethod-> child->type) + 1);
                    auxTableNode -> next = createSymbolTableNode(typeParams, auxMethod-> child->next-> content, NULL, "param", 0);
                    auxTableNode = auxTableNode -> next;
                }
                auxMethod = auxMethod->next;

                while(auxMethod != NULL) {
                    if(checkVariableExistance(auxMethod -> child -> next, auxTableMethodNode, auxTableMethodNode, 1, 0) != NULL) {
                        printf("Line %d, col %d: Symbol %s already defined\n", auxMethod -> child -> next -> line, auxMethod -> child -> next -> column, auxMethod -> child -> next -> content);

                        if(strcmp(auxMethod-> child->type, "Bool") == 0) {
                            sprintf(typeParams, "%s,%s", typeParams, "boolean");
                        } else {
                            sprintf(typeParams, "%s,%c%s", typeParams, (auxMethod-> child->type)[0] + 'a' - 'A', (auxMethod-> child->type) + 1);
                        }
                    } else {
                        if(strcmp(auxMethod-> child->type, "Bool") == 0) {
                            sprintf(typeParams, "%s,%s", typeParams, "boolean");
                            auxTableNode -> next = createSymbolTableNode("boolean", auxMethod-> child->next-> content, NULL, "param", 0);
                        } else {
                            sprintf(typeParams, "%s,%c%s", typeParams, (auxMethod-> child->type)[0] + 'a' - 'A', (auxMethod-> child->type) + 1);
                            auxTableNode -> next = createSymbolTableNode(auxMethod-> child->type, auxMethod-> child->next-> content, NULL, "param", 0);
                        }
                        auxTableNode = auxTableNode -> next;
                    }

                    auxMethod = auxMethod->next;
                }

                char newTypeParams[1024];
                sprintf(newTypeParams, "(%s)", typeParams);

                auxTableMethodNode -> params = strdup(newTypeParams);
            } else {
                auxTableMethodNode -> params = strdup("()");
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
                aux -> flagSkip = 1;
                table = tablePrevious;
                table -> next = NULL;
                aux = aux -> next;
                continue;
            }

            aux = aux -> next;
        }
    }

    return root;
}

void printTable(SymbolTableNode *symbolTable) {
    SymbolTableNode *aux = symbolTable, *aux2;

    if(symbolTable-> child == NULL) {
        printf("===== Class %s Symbol Table =====\n", aux -> name);
        return;
    }

    while(aux != NULL) {
        if(strcmp(aux -> type, "Class") == 0) {
            printf("===== Class %s Symbol Table =====\n", aux -> name);
            aux = aux -> child;
        } else {
            if(aux -> flag == NULL) {
                if(aux -> params == NULL) {
                    printf("%s\t\t%s\n", aux -> name, aux -> type);
                } else {
                    printf("%s\t%s\t%s\n", aux -> name, aux -> params, aux -> type);
                }
            } else {
                if(aux -> params == NULL) {
                    printf("%s\t\t%s\t%s\n", aux -> name, aux -> type, aux -> flag);
                } else {
                    printf("%s\t%s\t%s\t%s\n", aux -> name, aux -> params, aux -> type, aux -> flag);
                }
            }
            aux = aux -> next;
        }
    }

    aux = symbolTable-> child;

    while(aux != NULL) {
        if(aux -> flagMethod) {
            printf("\n===== Method %s%s Symbol Table =====\n", aux -> name, aux -> params);

            aux2 = aux -> child;
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

        aux = aux -> next;
    }
}

void destroySymbolTable(SymbolTableNode *symbolTable) {
    if(symbolTable == NULL)
        return;

    free(symbolTable->type);
    free(symbolTable->name);
    if(symbolTable->params != NULL)
        free(symbolTable->params);
    if(symbolTable->flag != NULL)
        free(symbolTable->flag);

    destroySymbolTable(symbolTable-> child);
    destroySymbolTable(symbolTable->next);
}

void ASTSemanticAnnotations(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable) {
    ASTNode * child1, * child2;
    SymbolTableNode * auxSymbolTable;
    char * varType;

    if(node == NULL) {
        return;
    }

    if(node -> annotation != NULL) {
        return;
    }

    if(node -> flagSkip) {
        ASTSemanticAnnotations(node -> next, symbolTable, 0);
        return;
    }

    if(flagVariable) {
        if(strcmp(node -> type, "Id") == 0) {

            varType = checkVariableExistance(node, symbolTable, AnnotationscurrentMethodNode, 1, 1);

            if(varType != NULL) {
                node -> annotation = strdup(varType);
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Cannot find symbol %s\n", node -> line, node -> column, node -> content);
            }
            return;
        } else if(strcmp(node -> type, "DecLit") == 0) {

            char * number = parseNumbers(node -> content, 0);

            if(strlen(number) > 10 || atol(number) >= 2147483648) {
                printf("Line %d, col %d: Number %s out of bounds\n", node -> line, node -> column, node -> content);
            }
            free(number);
            node -> annotation = strdup("int");
            return;
        } else if(strcmp(node -> type, "RealLit") == 0) {

            char * decimal = strdup(node -> content);
            char * integer = NULL;
            integer = strsep(&decimal, ".");

            char * number = parseNumbers(integer, 1);
/*
            if (strlen(number) >=17) {
                char aux = number[17];
                number[17] = '\0';
            }
*/
            if(strlen(number) > 309 || atoll(number) > 17976931348623157) {
                printf("Line %d, col %d: Number %s out of bounds\n", node -> line, node -> column, node -> content);
            }

            free(number);
            free(integer);

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

        if(strcmp(child1 -> annotation, "undef") != 0 && strcmp(child2 -> annotation, "undef") != 0 && strcmp(child1 -> annotation, "String[]") != 0 && strcmp(child2 -> annotation, "String[]") != 0) {
            if(strcmp(child1 -> annotation, child2 -> annotation) == 0) {
                node -> annotation = strdup(child1 -> annotation);
            } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {
                node -> annotation = strdup("double");
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator = cannot be applied to types %s, %s\n", node -> line, node -> column, child1 -> annotation, child2 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator = cannot be applied to types %s, %s\n", node -> line, node -> column, child1 -> annotation, child2 -> annotation);
        }
    } else if(strcmp(node -> type, "Call") == 0) {
        child1 = node -> child;
        ASTNode * params = child1 -> next;

        node -> line = child1->line;
        node -> column = child1-> column;

        while(params != NULL) {
            ASTSemanticAnnotations(params, symbolTable, 1);
            params = params -> next;
        }

        char * methodType = getMethodType(child1, symbolTable);

        if(methodType == NULL) {
            printf("Line %d, col %d: Cannot find symbol %s(", node -> child -> line, node -> child -> column, node -> child -> content);
            params = child1 -> next;
            while(params != NULL) {
                printf("%s", params -> annotation);
                if(params -> next != NULL) printf(",");
                params = params -> next;
            }
            printf(")\n");
            node -> annotation = strdup("undef");
        } else if(strcmp(methodType, "ambiguous") == 0) {
            printf("Line %d, col %d: Reference to method %s(", node -> child -> line, node -> child -> column, node -> child -> content);
            params = child1 -> next;
            while(params != NULL) {
                printf("%s", params -> annotation);
                if(params -> next != NULL) printf(",");
                params = params -> next;
            }
            printf(") is ambiguous\n");
            child1 -> annotation = strdup("undef");
            node -> annotation = strdup("undef");
        } else {
            node -> annotation = strdup(methodType);
        }

    } else if(strcmp(node -> type, "ParseArgs") == 0) {
        child1 = node -> child;
        child2 = node -> child->next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(child2, symbolTable, 1);

        if(strcmp(child1 -> annotation, "String[]") != 0 || strcmp(child2 -> annotation, "int") != 0) {
            printf("Line %d, col %d: Operator Integer.parseInt cannot be applied to types %s, %s\n", node -> line, node -> column, child1 -> annotation, child2 -> annotation);
        }
        node -> annotation = strdup("int");
    } else if(strcmp(node -> type, "And") == 0 || strcmp(node -> type, "Or") == 0 || strcmp(node -> type, "Eq") == 0 || strcmp(node -> type, "Geq") == 0 || strcmp(node -> type, "Gt") == 0 || strcmp(node -> type, "Leq") == 0 || strcmp(node -> type, "Lt") == 0 || strcmp(node -> type, "Neq") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(child2, symbolTable, 1);

        char * operator = NULL;

        if(strcmp(node -> type, "And") == 0) {
            operator = strdup("&&");
        } else if(strcmp(node -> type, "Or") == 0) {
            operator = strdup("||");
        } else if(strcmp(node -> type, "Eq") == 0) {
            operator = strdup("==");
        } else if(strcmp(node -> type, "Geq") == 0) {
            operator = strdup(">=");
        } else if(strcmp(node -> type, "Gt") == 0) {
            operator = strdup(">");
        } else if(strcmp(node -> type, "Leq") == 0) {
            operator = strdup("<=");
        } else if(strcmp(node -> type, "Lt") == 0) {
            operator = strdup("<");
        } else if(strcmp(node -> type, "Neq") == 0) {
            operator = strdup("!=");
        }

        if(strcmp(child1 -> annotation, "undef") != 0 && strcmp(child2 -> annotation, "undef") != 0 && strcmp(child1 -> annotation, "String[]") != 0 && strcmp(child2 -> annotation, "String[]") != 0) {
            if(strcmp(child1 -> annotation, child2 -> annotation) == 0) {
                node -> annotation = strdup("boolean");
            } else if((strcmp(child1 -> annotation, "int") == 0 || strcmp(child1 -> annotation, "double") == 0) && (strcmp(child2 -> annotation, "int") == 0 || strcmp(child2 -> annotation, "double") == 0)) {
                node -> annotation = strdup("boolean");
            } else {
                node -> annotation = strdup("boolean");
                printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", node -> line, node -> column, operator, child1 -> annotation, child2 -> annotation);
            }
        } else {
            node -> annotation = strdup("boolean");
        }

        free(operator);
    } else if(strcmp(node -> type, "Add") == 0 || strcmp(node -> type, "Sub") == 0 || strcmp(node -> type, "Mul") == 0 || strcmp(node -> type, "Div") == 0 || strcmp(node -> type, "Mod") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(child2, symbolTable, 1);

        char operator = '\0';

        if(strcmp(node -> type, "Add") == 0) {
            operator = '+';
        } else if(strcmp(node -> type, "Sub") == 0) {
            operator = '-';
        } else if(strcmp(node -> type, "Mul") == 0) {
            operator = '*';
        } else if(strcmp(node -> type, "Div") == 0) {
            operator = '/';
        } else if(strcmp(node -> type, "Mod") == 0) {
            operator = '%';
        }

        if(strcmp(child1 -> annotation, "undef") != 0 && strcmp(child2 -> annotation, "undef") != 0 && strcmp(child1 -> annotation, "String[]") != 0 && strcmp(child2 -> annotation, "String[]") != 0) {
            if(strcmp(child1 -> annotation, child2 -> annotation) == 0) {
                node -> annotation = strdup(child1 -> annotation);
            } else if((strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) || (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0)) {
                node -> annotation = strdup("double");
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator %c cannot be applied to types %s, %s\n", node -> line, node -> column, operator, child1 -> annotation, child2 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator %c cannot be applied to types %s, %s\n", node -> line, node -> column, operator, child1 -> annotation, child2 -> annotation);
        }
    } else if(strcmp(node -> type, "Minus") == 0 || strcmp(node -> type, "Plus") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);

        char operator = '\0';

        if(strcmp(node -> type, "Plus") == 0) {
            operator = '+';
        } else if(strcmp(node -> type, "Minus") == 0) {
            operator = '-';
        }


        if(strcmp(child1 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, "int") == 0 || strcmp(child1 -> annotation, "double") == 0) {
                node -> annotation = strdup(child1 -> annotation);
            } else {
                node -> annotation = strdup("undef");
                printf("Line %d, col %d: Operator %c cannot be applied to type %s\n", node -> line, node -> column, operator, child1 -> annotation);
            }
        } else {
            node -> annotation = strdup("undef");
            printf("Line %d, col %d: Operator %c cannot be applied to type %s\n", node -> line, node -> column, operator, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "Not") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "undef") != 0) {
            if(strcmp(child1 -> annotation, "boolean") != 0) {
                printf("Line %d, col %d: Operator ! cannot be applied to type %s\n", node -> line, node -> column, child1 -> annotation);
            }
        } else {
            printf("Line %d, col %d: Operator ! cannot be applied to type %s\n", node -> line, node -> column, child1 -> annotation);
        }

        node -> annotation = strdup("boolean");
    } else if(strcmp(node -> type, "Print") == 0) {
        child1 = node -> child;
        ASTSemanticAnnotations(child1, symbolTable, 1);

        node -> line = child1 -> line;
        node -> column = child1 -> column;

        if(strcmp(child1 -> annotation, "undef") == 0 || strcmp(child1 -> annotation, "String[]") == 0) {
            printf("Line %d, col %d: Incompatible type %s in System.out.println statement\n", node -> line, node -> column, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "Length") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "String[]") != 0) {
            printf("Line %d, col %d: Operator .length cannot be applied to type %s\n", node -> line, node -> column, child1 -> annotation);
        }
        node -> annotation = strdup("int");
    } else if(strcmp(node -> type, "Return") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);

        if(strcmp(AnnotationscurrentMethodNode -> type, "void") == 0) {
            if(child1 != NULL) {
                printf("Line %d, col %d: Incompatible type %s in return statement\n", child1 -> line, child1 -> column, child1 -> annotation);
            }
        } else {
            if(child1 != NULL) {
                if (strcmp(AnnotationscurrentMethodNode -> type, child1 -> annotation) != 0) {
                    if(!(strcmp(AnnotationscurrentMethodNode -> type, "double") == 0 && strcmp(child1 -> annotation, "int") == 0)) {
                        printf("Line %d, col %d: Incompatible type %s in return statement\n", child1 -> line, child1 -> column, child1 -> annotation);
                    }
                }
            } else {
                printf("Line %d, col %d: Incompatible type void in return statement\n", node -> line, node -> column);
            }
        }
    } else if(strcmp(node -> type, "MethodHeader") == 0) {
        if(AnnotationscurrentMethodNode == NULL)
            auxSymbolTable = symbolTable -> child;
        else {
            auxSymbolTable = AnnotationscurrentMethodNode -> next;
        }

        while(auxSymbolTable != NULL) {
            if(auxSymbolTable -> flagMethod) {
                AnnotationscurrentMethodNode = auxSymbolTable;
                break;
            }
            auxSymbolTable = auxSymbolTable -> next;
        }
    } else if(strcmp(node -> type, "If") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        if(strcmp(child1 -> annotation, "boolean") != 0) {
            printf("Line %d, col %d: Incompatible type %s in if statement\n", child1 -> line, child1 -> column, child1 -> annotation);
        }
        //if ((child1 -> next -> child != NULL && child1 -> next -> child -> annotation == NULL) || (child1 -> next -> next -> child != NULL && child1 -> next -> next -> child -> annotation == NULL))
            ASTSemanticAnnotations(child1 -> next, symbolTable, 0);
    } else if(strcmp(node -> type, "While") == 0) {
        child1 = node -> child;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        if(strcmp(child1 -> annotation, "boolean") != 0) {
            printf("Line %d, col %d: Incompatible type %s in while statement\n", child1 -> line, child1 -> column, child1 -> annotation);
        }
        //if (child1 -> next -> child != NULL && child1 -> next -> child -> annotation == NULL)
            ASTSemanticAnnotations(node -> child -> next, symbolTable, 0);
    } else if(strcmp(node -> type, "DoWhile") == 0) {
        child1 = node -> child -> next;

        ASTSemanticAnnotations(child1, symbolTable, 1);
        ASTSemanticAnnotations(node -> child, symbolTable, 0);

        if(strcmp(child1 -> annotation, "boolean") != 0) {
            printf("Line %d, col %d: Incompatible type %s in do statement\n", child1 -> line, child1 -> column, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "VarDecl") == 0) {
        if(checkVariableExistance(node -> child -> next, symbolTable, AnnotationscurrentMethodNode, 1, 0) != NULL) {
            printf("Line %d, col %d: Symbol %s already defined\n", node -> child -> next -> line, node -> child -> next -> column, node -> child -> next -> content);
            ASTSemanticAnnotations(node -> next, symbolTable, 0);
            return;
        }

        SymbolTableNode * stnaux = AnnotationscurrentMethodNode -> child;
        while(stnaux -> next != NULL) {
            stnaux = stnaux -> next;
        }

        if(strcmp(node -> child -> type, "Bool") == 0) {
            stnaux -> next = createSymbolTableNode("boolean", node -> child -> next -> content, NULL, NULL, 0);
        } else {
            stnaux -> next = createSymbolTableNode(node -> child -> type, node -> child -> next -> content, NULL, NULL, 0);
        }
    } else {
        ASTSemanticAnnotations(node -> child, symbolTable, 0);
    }

    ASTSemanticAnnotations(node -> next, symbolTable, 0);
}

char * checkVariableExistance(ASTNode * astnode, SymbolTableNode * stnode, SymbolTableNode * currentMethodNode, int flagLocal, int flagGlobal) {
    char *variableName;
    SymbolTableNode *aux;

    if(stnode == NULL) {
        return NULL;
    }

    if(astnode -> content != NULL) {
        variableName = astnode -> content;
    } else {
        return NULL;
    }

    // check local variables
    if(flagLocal && currentMethodNode != NULL) {
        // check local variables
        aux = currentMethodNode -> child -> next;

        while(aux != NULL) {
            if(strcmp(aux -> name, variableName) == 0) {
                return aux -> type;
            }

            aux = aux -> next;
        }
    }

    // check global variables
    if(flagGlobal) {
        aux = stnode -> child;
        while(aux != NULL) {
            if(!(aux -> flagMethod) && strcmp(aux -> name, variableName) == 0) {
                return aux -> type;
            }

            aux = aux -> next;
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

    SymbolTableNode * lastFit = NULL;

    while(aux != NULL) {
        if(aux -> flagMethod && strcmp(astnode -> content, aux -> name) == 0) {
            ASTNode * params = astnode -> next;
            aux2 = aux -> child -> next;

            flagTaTudoBem = 1;
            flagPerfectFit = 1;
            while(params != NULL && aux2 != NULL && aux2 -> flag != NULL) {
                if(strcmp(params -> annotation, aux2 -> type) == 0) {
                    ;
                } else if(strcmp(params -> annotation, "int") == 0 && strcmp(aux2 -> type, "double") == 0) {
                    flagPerfectFit = 0;
                } else {
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

            if(params == NULL && (aux2 == NULL || aux2 -> flag == NULL)) {
                if(flagPerfectFit) {
                    astnode -> annotation = strdup(aux -> params);
                    return aux -> type;
                }

                if(lastFit == NULL) {
                    lastFit = aux;
                } else {
                    return "ambiguous";
                }
            }
        }
        aux = aux -> next;
    }

    if(lastFit != NULL) {
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

char * parseNumbers(char * number, int flagDouble) {
    int i, j = 0; //k = 0;
    char newNumber[5000];
//    char aux[50];

    if (!flagDouble) {
        for (i = 0; i < strlen(number); i++) {
            if (number[i] != '_') {
                newNumber[j] = number[i];
                j++;
            }
        }
        newNumber[j] = '\0';
    } else {
        for (i = 0; i < strlen(number); i++) {
            if (number[i] != '_') {
                newNumber[j] = number[i];
                j++;
            } else {
                break;
            }
        }
        if (number[i] == '.')
            i++;
        for (; i < strlen(number); i++) {
            if (number[i] != '_') {
                newNumber[j] = number[i];
                j++;
            } else {
                break;
            }
        }
/*
        else if (number[i] == 'e' || number[i] == 'E') {
            i++;
            if (number[i] == '+' || (number[i] >= '1' && number[i] <= '0')) {
                k = 0;
                if (number[i] == '+') {
                    i++;
                    for (; i < strlen(number); i++) {
                        if(number[i] != '_') {
                            aux[k] = number[i];
                            k++;
                        }
                    }
                    aux[k] = '\0';
                }
            }
            break;
        }
        for (i = 0; i < k; i++) {
            newNumber[j] = 0;
            j++;
        }
        newNumber[j] = '\0';
*/
        //1.7976931348623157E308
        //4.9E-324
    }

    return strdup(newNumber);
}

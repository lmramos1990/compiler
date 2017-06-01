#include "structs.h"

SymbolTableNode * annotationsCurrentMethodNode = NULL;
int variableMemoryCode = 1;
int stringCounter = 1;
int boolCounter = 1;
int logicalCounter = 1;
char *currentBlock;
int flowCounter = 1;


void generateIntermidiateRepresentation(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable) {
    ASTNode * child1, * child2;

    if (node == NULL) {
        return;
    }

    if (node -> flagSkip) {
        generateIntermidiateRepresentation(node -> next, symbolTable, 0);
        return;
    }

    if (flagVariable) {
        if (strcmp(node -> type, "Id") == 0) {
            char variableid[1024];

            if (node -> stnode -> flagMethod == 2) {
                sprintf(variableid, "%c%d%c", '%', variableMemoryCode, '\0');

                if (strcmp(node -> annotation, "int") == 0) {
                    printf(" %c%d = load i32, i32* @%s.%s\n", '%', variableMemoryCode, symbolTable -> name, node -> content);
                } else if (strcmp(node -> annotation, "double") == 0) {
                    printf(" %c%d = load double, double* @%s.%s\n", '%', variableMemoryCode, symbolTable -> name, node -> content);
                } else if (strcmp(node -> annotation, "boolean") == 0) {
                    printf(" %c%d = load i1, i1* @%s.%s\n", '%', variableMemoryCode, symbolTable -> name, node -> content);
                }

            } else {
                sprintf(variableid, "%c%d%c", '%', variableMemoryCode, '\0');

                if (strcmp(node -> annotation, "int") == 0) {
                    printf(" %c%d = load i32, i32* %c%s\n", '%', variableMemoryCode, '%', node -> content);
                } else if (strcmp(node -> annotation, "double") == 0) {
                    printf(" %c%d = load double, double* %c%s\n", '%', variableMemoryCode, '%', node -> content);
                } else if (strcmp(node -> annotation, "boolean") == 0) {
                    printf(" %c%d = load i1, i1* %c%s\n", '%', variableMemoryCode, '%', node -> content);
                } else if (strcmp(node -> annotation, "String[]") == 0) {
                    printf(" %c%d = load i32, i32* %c%s.c\n", '%', variableMemoryCode++, '%', node -> content);
                    printf(" %c%d = load i8**, i8*** %c%s\n", '%', variableMemoryCode, '%', node -> content);
                    sprintf(variableid, "%d%c", variableMemoryCode, '\0');
                }
            }

            variableMemoryCode++;
            if (node -> llvmCode != NULL)
                free(node -> llvmCode);
            node -> llvmCode = strdup(variableid);

            return;
        } else if (strcmp(node -> type, "DecLit") == 0) {
            char * number = removeUnderscores(node -> content);

            node -> llvmCode = strdup(number);

            free(number);
            return;
        } else if (strcmp(node -> type, "RealLit") == 0) {
            char * number = removeUnderscores(node -> content);

            number = recreateRealLit(number);

            node -> llvmCode = strdup(number);
            free(number);

            return;
        } else if (strcmp(node -> type, "BoolLit") == 0) {

            if (strcmp(node -> content, "true") == 0) {
                node -> llvmCode = strdup("1");
            } else {
                node -> llvmCode = strdup("0");
            }

            return;
        } else if (strcmp(node -> type, "String") == 0) {
            return;
        }
    }

    if (strcmp(node -> type, "Assign") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char variableid[1024];

        if (child1 -> stnode -> flagMethod == 2) {
            sprintf(variableid, "@%s.%s%c", symbolTable -> name, child1 -> content, '\0');
        } else {
            sprintf(variableid, "%c%s%c", '%', child1 -> content, '\0');
        }

        child1 -> llvmCode = strdup(variableid);

        if (strcmp(child1 -> annotation, "int") == 0) {
            printf(" store i32 %s, i32* %s\n", child2 -> llvmCode, child1 -> llvmCode);

        } else if (strcmp(child1 -> annotation, "double") == 0) {
            if (strcmp(child2 -> annotation, "int") == 0) {
                printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child2 -> llvmCode);
                free(child2 -> llvmCode);
                sprintf(variableid, "%c%d%c", '%', variableMemoryCode, '\0');
                child2 -> llvmCode = strdup(variableid);
                printf(" store double %c%d, double* %s\n", '%', variableMemoryCode++, child1 -> llvmCode);
            } else {
                printf(" store double %s, double* %s\n", child2 -> llvmCode, child1 -> llvmCode);
            }
        } else if (strcmp(child1 -> annotation, "boolean") == 0) {
            printf(" store i1 %s, i1* %s\n", child2 -> llvmCode, child1 -> llvmCode);
        }

        node -> llvmCode = strdup(child2 -> llvmCode);

    } else if (strcmp(node -> type, "Call") == 0) {
        child1 = node -> child;
        ASTNode * params = child1 -> next;
        SymbolTableNode * stnode = child1 -> stnode -> child -> next;
        char variableid[1024];
        int flagFirst;

        while (params != NULL) {
            generateIntermidiateRepresentation(params, symbolTable, 2);
            if (strcmp(params -> annotation, "int") == 0 && strcmp(stnode -> type, "double") == 0) {
                printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, params -> llvmCode);
                free(params -> llvmCode);
                sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
                params -> llvmCode = strdup(variableid);
            }

            params = params -> next;
            stnode = stnode -> next;
        }

        params = child1 -> next;
        stnode = child1 -> stnode -> child -> next;
        flagFirst = 1;

        char * parameters;
        if (strcmp(child1 -> stnode -> params, "(String[])") == 0) {
            parameters = strdup("stringarray");
        } else {
            parameters = parseParameters(child1 -> stnode -> params);
        }
        if (strcmp(child1 -> stnode -> type, "int") == 0)
            printf(" %c%d = call i32 @%s.%s.%s(", '%', variableMemoryCode, symbolTable -> name, child1 -> stnode -> name, parameters);
        else if (strcmp(child1 -> stnode -> type, "double") == 0)
            printf(" %c%d = call double @%s.%s.%s(", '%', variableMemoryCode, symbolTable -> name, child1 -> stnode -> name, parameters);
        else if (strcmp(child1 -> stnode -> type, "boolean") == 0)
            printf(" %c%d = call i1 @%s.%s.%s(", '%', variableMemoryCode, symbolTable -> name, child1 -> stnode -> name, parameters);
        else if (strcmp(child1 -> stnode -> type, "void") == 0)
            printf(" call void @%s.%s.%s(", symbolTable -> name, child1 -> stnode -> name, parameters);
        free(parameters);

        while (params != NULL) {
            if (!flagFirst)
                printf(", ");

            if (strcmp(stnode -> type, "int") == 0) {
                printf("i32 %s", params -> llvmCode);
            } else if (strcmp(stnode -> type, "double") == 0) {
                printf("double %s", params -> llvmCode);
            } else if (strcmp(stnode -> type, "boolean") == 0) {
                printf("i1 %s", params -> llvmCode);
            } else if (strcmp(stnode -> type, "String[]") == 0) {
                printf("i32 %c%d, i8** %c%d", '%', atoi(params -> llvmCode) - 1, '%', atoi(params -> llvmCode));
            }

            flagFirst = 0;
            stnode = stnode -> next;
            params = params -> next;
        }
        printf(")\n");

        if (strcmp(child1 -> stnode -> type, "void") != 0) {
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node->llvmCode = strdup(variableid);
        }

    } else if (strcmp(node -> type, "ParseArgs") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child2, symbolTable, 1);

        printf(" %c%d = load i8**, i8*** %c%s\n", '%', variableMemoryCode++, '%', child1 -> content);
        printf(" %c%d = add i32 1, %s\n", '%', variableMemoryCode++, child2 -> llvmCode);
        printf(" %c%d = getelementptr inbounds i8*, i8** %c%d, i32 %c%d\n", '%', variableMemoryCode, '%', variableMemoryCode - 2, '%', variableMemoryCode - 1);
        variableMemoryCode++;

        printf(" %c%d = load i8*, i8** %c%d\n", '%', variableMemoryCode, '%', variableMemoryCode - 1);
        variableMemoryCode++;

        printf(" %c%d = call i32 @atoi(i8* %c%d)\n", '%', variableMemoryCode, '%', variableMemoryCode - 1);

        char variableID[1024];
        sprintf(variableID, "%c%d%c", '%', variableMemoryCode++, '\0');
        node -> llvmCode = strdup(variableID);

    } else if (strcmp(node -> type, "Geq") == 0 || strcmp(node -> type, "Gt") == 0 || strcmp(node -> type, "Leq") == 0 || strcmp(node -> type, "Lt") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char * operator = NULL;

        if (strcmp(node -> type, "Geq") == 0) {
            operator = strdup("ge");
        } else if (strcmp(node -> type, "Gt") == 0) {
            operator = strdup("gt");
        } else if (strcmp(node -> type, "Leq") == 0) {
            operator = strdup("le");
        } else if (strcmp(node -> type, "Lt") == 0) {
            operator = strdup("lt");
        }

        char variableid[1024];

        if (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "int") == 0) {

            printf(" %c%d = icmp s%s i32 %s, %s\n", '%', variableMemoryCode, operator, child1 -> llvmCode, child2 -> llvmCode);
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        } else if (strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "double") == 0) {

            printf(" %c%d = fcmp o%s double %s, %s\n", '%', variableMemoryCode, operator, child1 -> llvmCode, child2 -> llvmCode);
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        } else if (strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {

            printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child2 -> llvmCode);
            printf(" %c%d = fcmp o%s double %s, %c%d\n", '%', variableMemoryCode + 1, operator, child1 -> llvmCode, '%', variableMemoryCode);
            variableMemoryCode++;
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        } else if (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0) {

            printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child1 -> llvmCode);
            printf(" %c%d = fcmp o%s double %c%d, %s\n", '%', variableMemoryCode + 1, operator, '%', variableMemoryCode, child2 -> llvmCode);
            variableMemoryCode++;
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        }

        free(operator);
    } else if (strcmp(node -> type, "Eq") == 0 || strcmp(node -> type, "Neq") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char * operator = NULL;

        if (strcmp(node -> type, "Eq") == 0) {
            if (strcmp(child1 -> annotation, child2 -> annotation) == 0 && strcmp(child1 -> annotation, "double") != 0) {
                operator = strdup("eq");
            } else {
                operator = strdup("oeq");
            }
        } else {
            if (strcmp(child1 -> annotation, child2 -> annotation) == 0 && strcmp(child1 -> annotation, "double") != 0) {
                operator = strdup("ne");
            } else {
                operator = strdup("une");
            }
        }

        char variableid[1024];

        if (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "int") == 0) {

            printf(" %c%d = icmp %s i32 %s, %s\n", '%', variableMemoryCode, operator, child1 -> llvmCode, child2 -> llvmCode);
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        } else if (strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "double") == 0) {

            printf(" %c%d = fcmp %s double %s, %s\n", '%', variableMemoryCode, operator, child1 -> llvmCode, child2 -> llvmCode);
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        } else if (strcmp(child1 -> annotation, "boolean") == 0 && strcmp(child2 -> annotation, "boolean") == 0) {

            printf(" %c%d = icmp %s i1 %s, %s\n", '%', variableMemoryCode, operator, child1 -> llvmCode, child2 -> llvmCode);
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        } else if (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0) {

            printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child1 -> llvmCode);
            printf(" %c%d = fcmp %s double %c%d, %s\n", '%', variableMemoryCode + 1, operator, '%', variableMemoryCode, child2 -> llvmCode);
            variableMemoryCode++;
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        } else if (strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {

            printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child2 -> llvmCode);
            printf(" %c%d = fcmp %s double %s, %c%d\n", '%', variableMemoryCode + 1, operator, child1 -> llvmCode, '%', variableMemoryCode);
            variableMemoryCode++;
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

        }

        free(operator);
    } else if (strcmp(node -> type, "And") == 0 || strcmp(node -> type, "Or") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 2);

        char variableid[1024];
        char *auxBlock;
        int auxCounter;

        auxBlock = strdup(currentBlock);

        if (strcmp(node -> type, "And") == 0) {
            auxCounter = logicalCounter++;

            printf(" br i1 %s, label %cand.true.%d, label %cand.end.%d\n", child1 -> llvmCode, '%', auxCounter, '%', auxCounter);

            printf("and.true.%d:\n", auxCounter);
            free(currentBlock);
            sprintf(variableid, "%cand.true.%d%c", '%', auxCounter, '\0');
            currentBlock = strdup(variableid);
            generateIntermidiateRepresentation(child2, symbolTable, 2);
            printf(" br label %cand.end.%d\n", '%', auxCounter);

            printf("and.end.%d:\n", auxCounter);
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);
            printf(" %s = phi i1 [%s, %s], [false, %s]\n", node -> llvmCode, child2 -> llvmCode, currentBlock, auxBlock);

            free(currentBlock);
            sprintf(variableid, "%cand.end.%d%c", '%', auxCounter, '\0');
            currentBlock = strdup(variableid);

        } else if (strcmp(node -> type, "Or") == 0) {
            auxCounter = logicalCounter++;

            printf(" br i1 %s, label %cor.end.%d, label %cor.false.%d\n", child1 -> llvmCode, '%', auxCounter, '%', auxCounter);

            printf("or.false.%d:\n", auxCounter);
            free(currentBlock);
            sprintf(variableid, "%cor.false.%d%c", '%', auxCounter, '\0');
            currentBlock = strdup(variableid);
            generateIntermidiateRepresentation(child2, symbolTable, 2);
            printf(" br label %cor.end.%d\n", '%', auxCounter);

            printf("or.end.%d:\n", auxCounter);
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);
            printf(" %s = phi i1 [%s, %s], [true, %s]\n", node -> llvmCode, child2 -> llvmCode, currentBlock, auxBlock);


            free(currentBlock);
            sprintf(variableid, "%cor.end.%d%c", '%', auxCounter, '\0');
            currentBlock = strdup(variableid);
        }

        free(auxBlock);
    } else if (strcmp(node -> type, "Add") == 0 || strcmp(node -> type, "Sub") == 0 || strcmp(node -> type, "Mul") == 0 || strcmp(node -> type, "Div") == 0 || strcmp(node -> type, "Mod") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char * operator = NULL;

        if (strcmp(node -> type, "Add") == 0) {
            operator = strdup("add");
        } else if (strcmp(node -> type, "Sub") == 0) {
            operator = strdup("sub");
        } else if (strcmp(node -> type, "Mul") == 0) {
            operator = strdup("mul");
        } else if (strcmp(node -> type, "Div") == 0) {
            operator = strdup("div");
        } else if (strcmp(node -> type, "Mod") == 0) {
            operator = strdup("rem");
        }

        char variableid[1024];

        if (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

            if (strcmp(node -> type, "Mod") == 0 || strcmp(node -> type, "Div") == 0) {
                printf(" %s = s%s i32 %s, %s\n", node  -> llvmCode, operator, child1 -> llvmCode, child2 -> llvmCode);
            } else {
                printf(" %s = %s i32 %s, %s\n", node  -> llvmCode, operator, child1 -> llvmCode, child2 -> llvmCode);
            }

        } else if (strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
            node -> llvmCode = strdup(variableid);

            printf(" %s = f%s double %s, %s\n", node  -> llvmCode, operator, child1 -> llvmCode, child2 -> llvmCode);

        } else if (strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child1 -> llvmCode);

            sprintf(variableid, "%c%d%c", '%', variableMemoryCode + 1, '\0');
            node -> llvmCode = strdup(variableid);

            printf(" %s = f%s double %c%d, %s\n", node  -> llvmCode, operator, '%', variableMemoryCode, child2 -> llvmCode);
            variableMemoryCode += 2;

        } else if (strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child2 -> llvmCode);

            sprintf(variableid, "%c%d%c", '%', variableMemoryCode + 1, '\0');
            node -> llvmCode = strdup(variableid);

            printf(" %s = f%s double %s, %c%d\n", node  -> llvmCode, operator, child1 -> llvmCode, '%', variableMemoryCode);
            variableMemoryCode += 2;
        }

        free(operator);
    } else if (strcmp(node -> type, "Minus") == 0 || strcmp(node -> type, "Plus") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        if (strcmp(node -> type, "Plus") == 0) {
            node -> llvmCode = child1 -> llvmCode;
        } else if (strcmp(node -> type, "Minus") == 0) {
            char variableCode[4];
            sprintf(variableCode, "%c%d", '%', variableMemoryCode++);
            node -> llvmCode = strdup(variableCode);

            if (strcmp(child1 -> annotation, "int") == 0) {
                printf(" %s = mul i32 -1, ", node -> llvmCode);
            } else if (strcmp(child1 -> annotation, "double") == 0) {
                printf(" %s = fmul double -1.0, ", node -> llvmCode);
            }

            printf("%s\n", child1 -> llvmCode);
        }
    } else if (strcmp(node -> type, "Not") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        char variableid[1024];

        sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
        node -> llvmCode = strdup(variableid);

        printf("%s = xor i1 %s, true\n", node -> llvmCode, child1 -> llvmCode);
    } else if (strcmp(node -> type, "Print") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        if (strcmp(child1 -> annotation, "int") == 0) {
            printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);
            printf("([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %s)\n", child1 -> llvmCode);
        } else if (strcmp(child1 -> annotation, "double") == 0) {
            printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);
            printf("([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %s)\n", child1 -> llvmCode);
        } else if (strcmp(child1 -> annotation, "boolean") == 0) {
            if (strcmp(child1 -> type, "BoolLit") == 0) {
                if (strcmp(child1 -> content, "true") == 0) {
                    printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);
                    printf("([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))\n");
                } else {
                    printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);
                    printf("([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))\n");
                }
            } else {
                printf(" br i1 %s, label %cboolean.true.%d, label %cboolean.false.%d\n", child1 -> llvmCode, '%', boolCounter, '%', boolCounter);

                printf("boolean.true.%d:\n", boolCounter);
                printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);
                printf("([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))\n");
                printf(" br label %cboolean.end.%d\n", '%', boolCounter);

                printf("boolean.false.%d:\n", boolCounter);
                printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);
                printf("([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))\n");
                printf(" br label %cboolean.end.%d\n", '%', boolCounter);

                printf("boolean.end.%d:\n", boolCounter);

                char variableid[1024];
                free(currentBlock);
                sprintf(variableid, "%cboolean.end.%d%c", '%', boolCounter, '\0');
                currentBlock = strdup(variableid);

                boolCounter++;
            }
        } else if (strcmp(child1 -> annotation, "String") == 0) {
            printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);
            printf("([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [%d x i8]* %s)\n", stringLength(child1 -> content), child1 -> llvmCode);
        }

    } else if (strcmp(node -> type, "Length") == 0) {
        child1 = node -> child;

        char variableid[1024];
        sprintf(variableid, "%c%d%c", '%', variableMemoryCode + 1, '\0');
        node -> llvmCode = strdup(variableid);

        printf(" %c%d = load i32, i32* %c%s.c\n", '%', variableMemoryCode++, '%', child1 -> content);
        printf(" %s = sub i32 %c%d, 1\n", node -> llvmCode, '%', variableMemoryCode - 1);
        variableMemoryCode++;
    } else if (strcmp(node -> type, "Return") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        if(child1 == NULL) {
            printf(" ret void\n");
        } else if (strcmp(child1 -> annotation, "int") == 0) {
            if (strcmp(annotationsCurrentMethodNode -> type, "double") == 0) {
                printf(" %c%d = sitofp i32 %s to double\n", '%', variableMemoryCode, child1 -> llvmCode);
                printf(" ret double %c%d\n", '%', variableMemoryCode);
                variableMemoryCode++;
            } else {
                printf(" ret i32 %s\n", child1 -> llvmCode);
            }
        } else if (strcmp(child1 -> annotation, "double") == 0) {
            printf(" ret double %s\n", child1 -> llvmCode);
        } else if (strcmp(child1 -> annotation, "boolean") == 0) {
            printf(" ret i1 %s\n", child1 -> llvmCode);
        }
        char variableid[1024];
        sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');
        currentBlock = strdup(variableid);
    } else if (strcmp(node -> type, "MethodDecl") == 0) {
        variableMemoryCode = 1;

        SymbolTableNode * auxSymbolTable = NULL;

        if (annotationsCurrentMethodNode == NULL)
            auxSymbolTable = symbolTable -> child;
        else {
            auxSymbolTable = annotationsCurrentMethodNode -> next;
        }

        while (auxSymbolTable != NULL) {
            if (auxSymbolTable -> flagMethod == 1) {
                annotationsCurrentMethodNode = auxSymbolTable;
                break;
            }
            auxSymbolTable = auxSymbolTable -> next;
        }

        char * parameters = NULL;

        if (strcmp(node -> stnode -> params, "(String[])") == 0) {
            parameters = strdup("stringarray");
        } else {
            parameters = parseParameters(node -> stnode -> params);
        }

        printf("\n");
        if (strcmp(node -> stnode -> child -> type, "int") == 0) {
            printf("define i32 @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        } else if (strcmp(node -> stnode -> child -> type, "double") == 0) {
            printf("define double @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        } else if (strcmp(node -> stnode -> child -> type, "boolean") == 0) {
            printf("define i1 @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        } else {
            printf("define void @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        }
        free(parameters);

        SymbolTableNode * aux = node -> stnode -> child -> next;

        if (aux == NULL) {
            printf("(");
        } else {
            int first = 0;

            printf("(");
            while (aux != NULL && aux -> flag != NULL && strcmp(aux -> flag, "param") == 0) {

                if (first) {
                    printf(", ");
                }

                if (strcmp(aux -> type, "boolean") == 0) {
                    printf("i1 %c%s.param", '%', aux -> name);
                } else if (strcmp(aux -> type, "int") == 0) {
                    printf("i32 %c%s.param", '%', aux -> name);
                } else if (strcmp(aux -> type, "double") == 0) {
                    printf("double %c%s.param", '%', aux -> name);
                } else if (strcmp(aux -> type, "String[]") == 0) {
                    printf("i32 %c%s.c.param, i8** %c%s.param", '%', aux -> name, '%', aux -> name);
                }

                first++;
                aux = aux -> next;
            }
        }
        printf(") {\n");

        aux = node -> stnode -> child -> next;

        while (aux != NULL && aux -> flag != NULL && strcmp(aux -> flag, "param") == 0) {
            if (strcmp(aux -> type, "boolean") == 0) {

                printf(" %c%s = alloca i1\n", '%', aux -> name);
                printf(" store i1 %c%s.param, i1* %c%s\n", '%', aux -> name, '%', aux -> name);

            } else if (strcmp(aux -> type, "int") == 0) {

                printf(" %c%s = alloca i32\n", '%', aux -> name);
                printf(" store i32 %c%s.param, i32* %c%s\n", '%', aux -> name, '%', aux -> name);

            } else if (strcmp(aux -> type, "double") == 0) {

                printf(" %c%s = alloca double\n", '%', aux -> name);
                printf(" store double %c%s.param, double* %c%s\n", '%', aux -> name, '%', aux -> name);

            } else if (strcmp(aux -> type, "String[]") == 0) {
                printf(" %c%s.c = alloca i32\n", '%', aux -> name);
                printf(" %c%s = alloca i8**\n", '%', aux -> name);
                printf(" store i32 %c%s.c.param, i32* %c%s.c\n", '%', aux -> name, '%', aux -> name);
                printf(" store i8** %c%s.param, i8*** %c%s\n", '%', aux -> name, '%', aux -> name);
            }

            aux = aux -> next;
        }

        declareVariables(node -> child);

        if (currentBlock != NULL)
            free(currentBlock);
        currentBlock = strdup("%0");

        generateIntermidiateRepresentation(node -> child -> next, symbolTable, 0);

    } else if (strcmp(node -> type, "If") == 0) {
        child1 = node -> child;

        char variableid[1024];
        int auxCounter = flowCounter++;

        generateIntermidiateRepresentation(child1, symbolTable, 2);

        printf(" br i1 %s, label %cif.true.%d, label %cif.false.%d\n", child1 -> llvmCode, '%', auxCounter, '%', auxCounter);

        printf("if.true.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cif.true.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);
        generateIntermidiateRepresentation(child1 -> next, symbolTable, 2);
        printf(" br label %cif.end.%d\n", '%', auxCounter);

        printf("if.false.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cif.false.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);
        generateIntermidiateRepresentation(child1 -> next -> next, symbolTable, 2);
        printf(" br label %cif.end.%d\n", '%', auxCounter);

        printf("if.end.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cif.end.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);

    } else if (strcmp(node -> type, "While") == 0) {
        child1 = node -> child;

        char variableid[1024];
        int auxCounter = flowCounter++;

        printf(" br label %cwhile.start.%d\n", '%', auxCounter);

        printf("while.start.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cwhile.start.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);
        generateIntermidiateRepresentation(child1, symbolTable, 2);
        printf(" br i1 %s, label %cwhile.true.%d, label %cwhile.end.%d\n", child1 -> llvmCode, '%', auxCounter, '%', auxCounter);

        printf("while.true.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cwhile.true.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);
        generateIntermidiateRepresentation(child1 -> next, symbolTable, 2);
        printf(" br label %cwhile.start.%d\n", '%', auxCounter);

        printf("while.end.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cwhile.end.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);

    } else if (strcmp(node -> type, "DoWhile") == 0) {
        child1 = node -> child -> next;

        char variableid[1024];
        int auxCounter = flowCounter++;

        printf(" br label %cdowhile.true.%d\n", '%', auxCounter);

        printf("dowhile.true.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cdowhile.true.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);
        generateIntermidiateRepresentation(child1, symbolTable, 2);
        printf(" br label %cdowhile.start.%d\n", '%', auxCounter);

        printf("dowhile.start.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cdowhile.start.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);
        generateIntermidiateRepresentation(node -> child, symbolTable, 1);
        printf(" br i1 %s, label %cdowhile.true.%d, label %cdowhile.end.%d\n", child1 -> llvmCode, '%', auxCounter, '%', auxCounter);

        printf("dowhile.end.%d:\n", auxCounter);
        free(currentBlock);
        sprintf(variableid, "%cdowhile.end.%d%c", '%', auxCounter, '\0');
        currentBlock = strdup(variableid);


    } else if (strcmp(node -> type, "Program") == 0) {
        printf("define i32 @main(i32 %cargc, i8** %cargv) {\n call void @%s.main.stringarray(i32 %cargc, i8** %cargv)\n ret i32 0\n}\n", '%', '%', symbolTable -> name, '%', '%');

        printf("\ndeclare i32 @printf(i8*, ...)\n");
        printf("declare i32 @atoi(i8*)\n");
        doStrings(node);

        printf("@str.str = private unnamed_addr constant [4 x i8] c\"%cs\\0A\\00\"\n", '%');
        printf("@str.int = private unnamed_addr constant [4 x i8] c\"%cd\\0A\\00\"\n", '%');
        printf("@str.double = private unnamed_addr constant [7 x i8] c\"%c.16E\\0A\\00\"\n", '%');
        printf("@str.true = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n");
        printf("@str.false = private unnamed_addr constant [7 x i8] c\"false\\0A\\00\"\n");
        printf("\n");

        generateIntermidiateRepresentation(node -> child, symbolTable, 0);

        free(currentBlock);
    } else if (strcmp(node -> type, "MethodBody") == 0) {
        generateIntermidiateRepresentation(node -> child, symbolTable, 0);

        if (strcmp(annotationsCurrentMethodNode -> type, "void") == 0) {
            printf(" ret void\n");
        } else if (strcmp(annotationsCurrentMethodNode -> type, "double") == 0) {
            printf(" ret double 0.0\n");
        } else if(strcmp(annotationsCurrentMethodNode -> type, "int") == 0) {
            printf(" ret i32 0\n");
        } else if (strcmp(annotationsCurrentMethodNode -> type, "boolean") == 0) {
            printf(" ret i1 1\n");
        }

        printf("}\n");

    } else if (strcmp(node -> type, "FieldDecl") == 0) {
        if (strcmp(node -> child -> type, "Int") == 0) {
            printf("@%s.%s = common global i32 0\n", symbolTable -> name, node -> child -> next -> content);
        } else if (strcmp(node -> child -> type, "Double") == 0) {
            printf("@%s.%s = common global double 0.0\n", symbolTable -> name, node -> child -> next -> content);
        } else if (strcmp(node -> child -> type, "Bool") == 0) {
            printf("@%s.%s = common global i1 0\n", symbolTable -> name, node -> child -> next -> content);
        }
    } else {
        generateIntermidiateRepresentation(node -> child, symbolTable, 0);
    }

    if (flagVariable != 2) {
        generateIntermidiateRepresentation(node -> next, symbolTable, 0);
    }
}

char * removeUnderscores(char * number) {
    int i, j = 0;
    char newnumber[5000];

    for (i = 0; i < strlen(number); i++) {
        if (number[i] != '_') {
            newnumber[j] = number[i];
            j++;
        }
    }
    newnumber[j] = '\0';

    return strdup(newnumber);
}

void doStrings(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    if (strcmp(node -> type, "StrLit") == 0) {
        char variableID[1024];
        int i;

        sprintf(variableID, "@str.%d", stringCounter++);

        node -> llvmCode = strdup(variableID);
        printf("%s = private unnamed_addr constant [%d x i8] c\"", node -> llvmCode, stringLength(node -> content));

        for (i = 1; i < strlen(node -> content) - 1; i++) {
            if (node -> content[i] == '\\') {
                if (node -> content[i + 1] == 'n') {
                    printf("\\0A");
                } else if (node -> content[i + 1] == 't') {
                    printf("\\09");
                } else if (node -> content[i + 1] == '\\') {
                    printf("\\5C");
                } else if (node -> content[i + 1] == '\"') {
                    printf("\\22");
                } else if (node -> content[i + 1] == 'r') {
                    printf("\\0D");
                } else if (node -> content[i + 1] == 'f') {
                    printf("\\0C");
                }
                i++;
            } else {
                printf("%c", node -> content[i]);
            }
        }
        printf("\\00\"\n");
    }

    doStrings(node -> child);
    doStrings(node -> next);
}

void declareVariables(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    if (strcmp(node -> type, "VarDecl") == 0) {
        if (strcmp(node -> child -> type, "Int") == 0) {
            printf(" %c%s = alloca i32\n", '%', node -> child -> next -> content);
        } else if (strcmp(node -> child -> type, "Double") == 0) {
            printf(" %c%s = alloca double\n", '%', node -> child -> next -> content);
        } else if (strcmp(node -> child -> type, "Bool") == 0) {
            printf(" %c%s = alloca i1\n", '%', node -> child -> next -> content);
        }
    }

    declareVariables(node -> child);
    declareVariables(node -> next);
}

char * parseParameters(char * params) {
    char parameters[1024];
    int i, j = 0;

    for (i = 1; i < strlen(params) - 1; i++) {
        if (params[i] != ',') {
            parameters[j] = params[i];
            j++;
        }
    }
    parameters[j] = '\0';

    return strdup(parameters);
}

char * recreateRealLit(char * number) {
    int i, isr = 1, point = 0, epos = -1;
    char newnumber[1024];

    for (i = 0; i < strlen(number); i++) {
        if (number[i] == '.') {
            point = 1;
            if (i == 0) {
                isr = 0;
            }
        } else if (number[i] == 'e' || number[i] == 'E') {
            epos = i;
        }
    }

    if (!point) {
        int j = 0;
        for (i = 0; i < strlen(number); i++) {
            if (i == epos) {
                newnumber[j++] = '.';
                newnumber[j] = number[i];
            } else {
                newnumber[j] = number[i];
            }
            j++;
        }
        newnumber[j] = '\0';
        return strdup(newnumber);
    }

    if (!isr) {
        int j = 1;
        newnumber[0] = '0';
        for (i = 0; i < strlen(number); i++) {
            newnumber[j] = number[i];
            j++;
        }
        newnumber[j] = '\0';

        return strdup(newnumber);
    }

    return number;
}

int stringLength(char * string) {
    int i, counter = 0;

    for (i = 1; i < strlen(string) - 1; i++) {
        if (string[i] == '\\') {
            i++;
        }
        counter++;
    }

    return counter + 1;
}

#include "structs.h"

SymbolTableNode * annotationsCurrentMethodNode = NULL;
int variableMemoryCode = 1;
int stringCounter = 1;

void generateIntermidiateRepresentation(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable) {
    ASTNode * child1, * child2;

    if(node == NULL) {
        return;
    }

    if(node -> flagSkip) {
        generateIntermidiateRepresentation(node -> next, symbolTable, 0);
        return;
    }

    if(flagVariable) {
        if(strcmp(node -> type, "Id") == 0) {
            char variableid[1024];

            if(node -> stnode -> flagMethod == 2) {
                sprintf(variableid, "%c%d%c", '%', variableMemoryCode, '\0');

                if(strcmp(node -> annotation, "int") == 0) {
                    printf(" %c%d = load i32, i32* @%s\n", '%', variableMemoryCode, node -> content);
                } else if(strcmp(node -> annotation, "double") == 0) {
                    printf(" %c%d = load double, double* @%s\n", '%', variableMemoryCode, node -> content);
                } else if(strcmp(node -> annotation, "boolean") == 0) {
                    printf(" %c%d = load i1, i1* @%s\n", '%', variableMemoryCode, node -> content);
                }

            } else {
                // ver o tipo da variavel e double - usar ponteiro para symbol table
                sprintf(variableid, "%c%d%c", '%', variableMemoryCode, '\0');

                printf("t %s c %s a %s l %s\n", node -> type, node -> content, node -> annotation, node -> llvmCode);

                if(strcmp(node -> annotation, "int") == 0) {
                    printf(" %c%d = load i32, i32* %c%s\n", '%', variableMemoryCode, '%', node -> content);
                } else if(strcmp(node -> annotation, "double") == 0) {
                    printf(" %c%d = load double, double* %c%s\n", '%', variableMemoryCode, '%', node -> content);
                } else if(strcmp(node -> annotation, "boolean") == 0) {
                    printf(" %c%d = load i1, i1* %c%s\n", '%', variableMemoryCode, '%', node -> content);
                }
            }

            variableMemoryCode++;
            node -> llvmCode = strdup(variableid);

            return;
        } else if(strcmp(node -> type, "DecLit") == 0) {
            char * number = removeUnderscores(node -> content);

            node -> llvmCode = strdup(number);

            free(number);
            return;
        } else if(strcmp(node -> type, "RealLit") == 0) {
            char * number = removeUnderscores(node -> content);

            node -> llvmCode = strdup(number);

            free(number);

            return;
        } else if(strcmp(node -> type, "BoolLit") == 0) {

            if(strcmp(node -> content, "true") == 0) {
                node -> llvmCode = strdup("1");
            } else {
                node -> llvmCode = strdup("0");
            }

            return;
        }
    }

    if(strcmp(node -> type, "Assign") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char variableid[1024];

        if(child1 -> stnode -> flagMethod == 2) {
            sprintf(variableid, "@%s%c", child1 -> content, '\0');
        } else {
            sprintf(variableid, "%c%s%c", '%', child1 -> content, '\0');
        }

        child1 -> llvmCode = strdup(variableid);

        if(strcmp(child1 -> annotation, "int") == 0) {
            printf(" store i32 %s, i32* %s\n", child2 -> llvmCode, child1 -> llvmCode);
        } else if(strcmp(child1 -> annotation, "double") == 0) {

            if(strcmp(child2 -> type, "DecLit") == 0) {
                printf(" store double %s.0, double* %s\n", child2 -> llvmCode, child1 -> llvmCode);
            } else {
                printf(" store double %s, double* %s\n", child2 -> llvmCode, child1 -> llvmCode);
            }
        } else if(strcmp(child1 -> annotation, "boolean") == 0) {
            printf(" store i1 %s, i1* %s\n", child2 -> llvmCode, child1 -> llvmCode);
        }

    } else if(strcmp(node -> type, "Call") == 0) {
        child1 = node -> child;
        ASTNode * params = child1 -> next;

        node -> line = child1 -> line;
        node -> column = child1 -> column;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        while(params != NULL) {
            generateIntermidiateRepresentation(params, symbolTable, 1);
            params = params -> next;
        }

        // char * methodType = getMethodType(child1, symbolTable);
        //
        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

    } else if(strcmp(node -> type, "ParseArgs") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        if(strcmp(child1 -> annotation, "String[]") != 0 || strcmp(child2 -> annotation, "int") != 0) {
            // printf("Line %d, col %d: Operator Integer.parseInt cannot be applied to types %s, %s\n", node -> line, node -> column, child1 -> annotation, child2 -> annotation);
        }
    } else if(strcmp(node -> type, "Geq") == 0 || strcmp(node -> type, "Gt") == 0 || strcmp(node -> type, "Leq") == 0 || strcmp(node -> type, "Lt") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        char * operator = NULL;

        if(strcmp(node -> type, "Geq") == 0) {
            operator = strdup(">=");
        } else if(strcmp(node -> type, "Gt") == 0) {
            operator = strdup(">");
        } else if(strcmp(node -> type, "Leq") == 0) {
            operator = strdup("<=");
        } else if(strcmp(node -> type, "Lt") == 0) {
            operator = strdup("<");
        }

        if(strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            ;   // int = int
        } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            ;   // double = double
        } else if(strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            ;   // int - double
        } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            ;   // double = int
        }

        // node -> annotation = strdup("boolean");

        free(operator);
    } else if(strcmp(node -> type, "Eq") == 0 || strcmp(node -> type, "Neq") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char * operator = NULL;

        if(strcmp(node -> type, "Eq") == 0) {
            operator = strdup("==");
        } else {
            operator = strdup("!=");
        }

        if(strcmp(child1 -> annotation, child2 -> annotation) != 0 || strcmp(child1 -> annotation, "String[]") == 0 || strcmp(child1 -> annotation, "undef") == 0 || strcmp(child1 -> annotation, "void") == 0) {
            if(!((strcmp(child1 -> annotation, "int") == 0 || strcmp(child1 -> annotation, "double") == 0) && (strcmp(child2 -> annotation, "int") == 0 || strcmp(child2 -> annotation, "double") == 0))) {
                // printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", node -> line, node -> column, operator, child1 -> annotation, child2 -> annotation);
            }
        }

        // node -> annotation = strdup("boolean");

        free(operator);
    } else if(strcmp(node -> type, "And") == 0 || strcmp(node -> type, "Or") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char * operator = NULL;

        if(strcmp(node -> type, "And") == 0) {
            operator = strdup("&&");
        } else if(strcmp(node -> type, "Or") == 0) {
            operator = strdup("||");
        }

        if(strcmp(child1 -> annotation, "boolean") != 0 || strcmp(child2 -> annotation, "boolean") != 0) {
            // printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", node -> line, node -> column, operator, child1 -> annotation, child2 -> annotation);
        }

        // node -> annotation = strdup("boolean");
    } else if(strcmp(node -> type, "Add") == 0 || strcmp(node -> type, "Sub") == 0 || strcmp(node -> type, "Mul") == 0 || strcmp(node -> type, "Div") == 0 || strcmp(node -> type, "Mod") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char * operator = NULL;

        if(strcmp(node -> type, "Add") == 0) {
            operator = strdup("add");
        } else if(strcmp(node -> type, "Sub") == 0) {
            operator = strdup("sub");
        } else if(strcmp(node -> type, "Mul") == 0) {
            operator = strdup("mul");
        } else if(strcmp(node -> type, "Div") == 0) {
            operator = strdup("div");
        } else if(strcmp(node -> type, "Mod") == 0) {
            operator = strdup("mod");
        }

        char variableid[1024];

        if(strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            printf(" %c%d = %s i32 %s, %s\n", '%', variableMemoryCode, operator, child1 -> llvmCode, child2 -> llvmCode);

            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');

            node -> llvmCode = strdup(variableid);
        } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            printf(" %c%d = %s double %s, %s\n", '%', variableMemoryCode++, operator, child1 -> llvmCode, child2 -> llvmCode);

            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');

            node -> llvmCode = strdup(variableid);
        } else if(strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            printf(" %c%d = %s double %s, %s\n", '%', variableMemoryCode++, operator, child1 -> llvmCode, child2 -> llvmCode);

            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');

            node -> llvmCode = strdup(variableid);
        } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            printf(" %c%d = %s double %s, %s\n", '%', variableMemoryCode++, operator, child1 -> llvmCode, child2 -> llvmCode);

            sprintf(variableid, "%c%d%c", '%', variableMemoryCode++, '\0');

            node -> llvmCode = strdup(variableid);
        }

        free(operator);
    } else if(strcmp(node -> type, "Minus") == 0 || strcmp(node -> type, "Plus") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        if(strcmp(node -> type, "Plus") == 0) {
            node -> llvmCode = child1 -> llvmCode;
        } else if(strcmp(node -> type, "Minus") == 0) {
            char variableCode[4];
            sprintf(variableCode, "%c%d", '%', variableMemoryCode++);
            node -> llvmCode = strdup(variableCode);

            if(strcmp(child1 -> annotation, "int") == 0) {
                printf(" %s = mul i32 -1, ", node -> llvmCode);
            } else if(strcmp(child1 -> annotation, "double") == 0) {
                printf(" %s = fmul double -1.0, ", node -> llvmCode);
            }

            printf("%s\n", child1 -> llvmCode);
        }
    } else if(strcmp(node -> type, "Not") == 0) {
        child1 = node -> child;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "boolean") != 0) {
            // printf("Line %d, col %d: Operator ! cannot be applied to type %s\n", node -> line, node -> column, child1 -> annotation);
        }

        // node -> annotation = strdup("boolean");
    } else if(strcmp(node -> type, "Print") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        printf(" %c%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ", '%', variableMemoryCode++);

        if(strcmp(child1 -> annotation, "int") == 0) {
            printf("([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %s)\n", child1 -> llvmCode);
        } else if(strcmp(child1 -> annotation, "double") == 0) {
            printf("([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %s)\n", child1 -> llvmCode);
        } else if(strcmp(child1 -> annotation, "boolean") == 0) {
            ;
        } else if(strcmp(child1 -> annotation, "String") == 0) {
            printf("([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [%ld x i8]* %s)\n", strlen(child1 -> content), child1 -> llvmCode);
        }

    } else if(strcmp(node -> type, "Length") == 0) {
        child1 = node -> child;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "String[]") != 0) {
            // printf("Line %d, col %d: Operator .length cannot be applied to type %s\n", node -> line, node -> column, child1 -> annotation);
        }

        // node -> annotation = strdup("int");
    } else if(strcmp(node -> type, "Return") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        if(strcmp(child1 -> annotation, "int") == 0) {
            printf(" ret i32 %s\n", child1 -> llvmCode);
        } else if(strcmp(child1 -> annotation, "double") == 0) {
            printf(" ret double %s\n", child1 -> llvmCode);
        } else if(strcmp(child1 -> annotation, "boolean") == 0) {
            printf(" ret i1 %s\n", child1 -> llvmCode);
        }

    } else if(strcmp(node -> type, "MethodDecl") == 0) {
        SymbolTableNode * auxSymbolTable = NULL;

        if(annotationsCurrentMethodNode == NULL)
            auxSymbolTable = symbolTable -> child;
        else {
            auxSymbolTable = annotationsCurrentMethodNode -> next;
        }

        while(auxSymbolTable != NULL) {
            if(auxSymbolTable -> flagMethod == 1) {
                annotationsCurrentMethodNode = auxSymbolTable;
                break;
            }
            auxSymbolTable = auxSymbolTable -> next;
        }

        char * parameters = NULL;

        if(strcmp(node -> stnode -> params, "(String[])") == 0) {
            parameters = strdup("stringarray");
        } else {
            parameters = parseParameters(node -> stnode -> params);
        }

        if(strcmp(node -> stnode -> child -> type, "int") == 0) {
            printf("define i32 @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        } else if(strcmp(node -> stnode -> child -> type, "double") == 0) {
            printf("define double @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        } else if(strcmp(node -> stnode -> child -> type, "boolean") == 0) {
            printf("define i1 @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        } else {
            printf("define void @%s.%s.%s", symbolTable -> name, node -> stnode -> name, parameters);
        }

        SymbolTableNode * aux = node -> stnode -> child -> next;

        if(aux == NULL) {
            printf("(");
        } else {
            int first = 0;

            printf("(");
            while(aux != NULL && aux -> flag != NULL && strcmp(aux -> flag, "param") == 0) {

                if(first) {
                    printf(", ");
                }

                if(strcmp(aux -> type, "boolean") == 0) {
                    printf("i1 %c%s", '%', aux -> name);
                } else if(strcmp(aux -> type, "int") == 0) {
                    printf("i32 %c%s", '%', aux -> name);
                } else if(strcmp(aux -> type, "double") == 0) {
                    printf("double %c%s", '%', aux -> name);
                } else if(strcmp(aux -> type, "String[]") == 0) {
                    printf("i32 %c%s.c, i8** %c%s", '%', aux -> name, '%', aux -> name);
                }

                first++;
                aux = aux -> next;
            }
        }
        printf(") {\n");



        generateIntermidiateRepresentation(node -> child -> next, symbolTable, 0);
    } else if(strcmp(node -> type, "If") == 0) {
        child1 = node -> child;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 2);

        if(strcmp(child1 -> annotation, "boolean") != 0) {
            // printf("Line %d, col %d: Incompatible type %s in if statement\n", child1 -> line, child1 -> column, child1 -> annotation);
        }

        generateIntermidiateRepresentation(child1 -> next, symbolTable, 0);
    } else if(strcmp(node -> type, "While") == 0) {
        child1 = node -> child;

        generateIntermidiateRepresentation(child1, symbolTable, 2);

        if(strcmp(child1 -> annotation, "boolean") != 0) {
            // printf("Line %d, col %d: Incompatible type %s in while statement\n", child1 -> line, child1 -> column, child1 -> annotation);
        }

        generateIntermidiateRepresentation(node -> child -> next, symbolTable, 0);
    } else if(strcmp(node -> type, "DoWhile") == 0) {
        child1 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 2);
        generateIntermidiateRepresentation(node -> child, symbolTable, 1);

        if(strcmp(child1 -> annotation, "boolean") != 0) {
            // printf("Line %d, col %d: Incompatible type %s in do statement\n", child1 -> line, child1 -> column, child1 -> annotation);
        }
    } else if(strcmp(node -> type, "VarDecl") == 0) {

        if(strcmp(node -> child -> type, "Int") == 0) {
            printf(" %c%s = alloca i32\n", '%', node -> child -> next -> content);
        } else if(strcmp(node -> child -> type, "Double") == 0) {
            printf(" %c%s = alloca double\n", '%', node -> child -> next -> content);
        } else if(strcmp(node -> child -> type, "Bool") == 0) {
            printf(" %c%s = alloca i1\n", '%', node -> child -> next -> content);
        }
    } else if(strcmp(node -> type, "Program") == 0) {
        printf("define i32 @main(i32 %cargc, i8** %cargv) {\n call void @%s.main.stringarray(i32 %cargc, i8** %cargv)\n ret i32 0\n}\n", '%', '%', symbolTable -> name, '%', '%');

        printf("\ndeclare i32 @printf(i8*, ...)\n");
        doStrings(node);

        printf("@str.str = private unnamed_addr constant [4 x i8] c\"%cs\\0A\\00\"\n", '%');
        printf("@str.int = private unnamed_addr constant [4 x i8] c\"%cd\\0A\\00\"\n", '%');
        printf("@str.double = private unnamed_addr constant [7 x i8] c\"%c.16E\\0A\\00\"\n", '%');
        printf("@str.true = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n");
        printf("@str.false = private unnamed_addr constant [7 x i8] c\"false\\0A\\00\"\n");
        printf("\n");

        generateIntermidiateRepresentation(node -> child, symbolTable, 0);

    } else if(strcmp(node -> type, "MethodBody") == 0) {
        generateIntermidiateRepresentation(node -> child, symbolTable, 0);

        if(strcmp(annotationsCurrentMethodNode -> type, "void") == 0) {
            printf(" ret void\n");
        }

        printf("}\n");

    } else if(strcmp(node -> type, "FieldDecl") == 0) {
        if(strcmp(node -> child -> type, "Int") == 0) {
            printf("@%s = common global i32 0\n", node -> child -> next -> content);
        } else if(strcmp(node -> child -> type, "Double") == 0) {
            printf("@%s = common global double 0.0\n", node -> child -> next -> content);
        } else if(strcmp(node -> child -> type, "Bool") == 0) {
            printf("@%s = common global i1 0\n", node -> child -> next -> content);
        }
    } else {
        generateIntermidiateRepresentation(node -> child, symbolTable, 0);
    }

    if(flagVariable != 2) {
        generateIntermidiateRepresentation(node -> next, symbolTable, 0);

    }
}

char * removeUnderscores(char * number) {
    int i, j = 0;
    char newnumber[5000];
    for(i = 0; i < strlen(number); i++) {
        if(number[i] != '_') {
            newnumber[j] = number[i];
            j++;
        }
    }

    newnumber[j] = '\0';

    return strdup(newnumber);
}

void doStrings(ASTNode *node) {
    if(node == NULL) {
        return;
    }

    if(strcmp(node -> type, "StrLit") == 0) {
        char variableID[1024];
        int i;

        sprintf(variableID, "@str.%d", stringCounter++);

        node -> llvmCode = strdup(variableID);
        printf("%s = private unnamed_addr constant [%ld x i8] c\"", node -> llvmCode, strlen(node -> content));

        for(i = 1; i < strlen(node -> content) - 1; i++) {
            printf("%c", node -> content[i]);
        }
        printf("\\0A\\00\"\n");
    }

    doStrings(node -> child);
    doStrings(node -> next);
}

char * parseParameters(char * params) {
    char parameters[1024];
    int i, j = 0;

    for(i = 1; i < strlen(params) - 1; i++) {
        if(params[i] != ',') {
            parameters[j] = params[i];
            j++;
        }
    }

    return strdup(parameters);
}

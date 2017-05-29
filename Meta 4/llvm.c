#include "structs.h"

SymbolTableNode * annotationsCurrentMethodNode = NULL;
int variableMemoryCode = 0;

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

void generateIntermidiateRepresentation(ASTNode * node, SymbolTableNode * symbolTable, int flagVariable) {
    ASTNode * child1, * child2;
    char * varType;

    if(node == NULL) {
        return;
    }

    if(node -> flagSkip) {
        generateIntermidiateRepresentation(node -> next, symbolTable, 0);
        return;
    }

    if(flagVariable) {
        if(strcmp(node -> type, "Id") == 0) {
            varType = checkVariableExistance(node, symbolTable, annotationsCurrentMethodNode, 1, 1);
            return;
        } else if(strcmp(node -> type, "DecLit") == 0) {
            char * number = removeUnderscores(node -> content);
            free(number);
            return;
        } else if(strcmp(node -> type, "RealLit") == 0) {
            char * number = removeUnderscores(node -> content);
            free(number);

            return;
        } else if(strcmp(node -> type, "BoolLit") == 0) {
            printf("boollit\n");
            return;
        } else if(strcmp(node -> type, "StrLit") == 0) {
            printf("strlit\n");
            return;
        }
    }

    if(strcmp(node -> type, "Assign") == 0) {
        child1 = node -> child;
        child2 = node -> child -> next;

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        // store i32 1, i32* %a
        // store i32 2, i32* %b
        // %0 = load i32, i32* %a
        // %1 = load i32, i32* %b

        // printf("CHILD1 %s\n", child1 -> content);
        // printf("CHILD2 %s\n", child2 -> content);

        if(strcmp(child1 -> annotation, "int") == 0) {
            // printf(" store i32 %s, i32* %c%s\n", child2 -> llvm, '%', child1 -> llvm);
            // printf(" %c%d = load i32, i32* %c%s\n", '%', variableMemoryCode, '%', child1 -> llvm);
            // variableMemoryCode++;
        } else if(strcmp(child1 -> annotation, "double") == 0) {
            // printf(" store double %s, double* %c%s\n", child2 -> llvm, '%', child1 -> content);
            // printf(" %c%d = load i32, i32* %c%s\n", '%', variableMemoryCode, '%', child1 -> content);
            // variableMemoryCode++;
        } else if(strcmp(child1 -> annotation, "boolean") == 0) {
            // printf(" store i1 %s, i1* %c%s\n", child2 -> llvm, '%', child1 -> content);
            // printf(" %c%d = load i32, i32* %c%s\n", '%', variableMemoryCode, '%', child1 -> content);
            // variableMemoryCode++;
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

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);
        generateIntermidiateRepresentation(child2, symbolTable, 1);

        char * operator = NULL;
        // char llvm[1024];

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

        if(strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            // int = int
            // printf("\t\thello world!\n");
            //
            // // %add = add i32 %0, %1
            // // store i32 %add, i32* %c
            //
            // printf(" %c%d = %s %c%s %c%s\n", '%', variableMemoryCode, operator, '%', child1 -> llvm, '%', child2 -> llvm);
            //
            // sprintf(llvm, "%c%d", '%', variableMemoryCode);
            // variableMemoryCode++;
            // node -> llvm = strdup(llvm);

        } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            // double = double
            // printf("hello world!\n");
            //
            // sprintf(llvm, "%s double %c%s %c%s%c", operator, '%', child1 -> content, '%', child2 -> content, '\0');
            // node -> llvm = strdup(llvm);

        } else if(strcmp(child1 -> annotation, "int") == 0 && strcmp(child2 -> annotation, "double") == 0) {
            // int - double
            // printf("hello world!\n");
            //
            // sprintf(llvm, "%s double %c%s %c%s%c", operator, '%', child1 -> content, '%', child2 -> content, '\0');
            // node -> llvm = strdup(llvm);

        } else if(strcmp(child1 -> annotation, "double") == 0 && strcmp(child2 -> annotation, "int") == 0) {
            // double = int
            // printf("hello world!\n");
            //
            // sprintf(llvm, "%s double %c%s %c%s%c", operator, '%', child1 -> content, '%', child2 -> content, '\0');
            // node -> llvm = strdup(llvm);
        }

        free(operator);
    } else if(strcmp(node -> type, "Minus") == 0 || strcmp(node -> type, "Plus") == 0) {
        child1 = node -> child;

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        char operator = '\0';
        if(strcmp(node -> type, "Plus") == 0) {
            operator = '+';
        } else if(strcmp(node -> type, "Minus") == 0) {
            operator = '-';
        }

        if(strcmp(child1 -> annotation, "int") == 0 || strcmp(child1 -> annotation, "double") == 0) {
            // node -> annotation = strdup(child1 -> annotation);
        } else {
            // node -> annotation = strdup("undef");
            // printf("Line %d, col %d: Operator %c cannot be applied to type %s\n", node -> line, node -> column, operator, child1 -> annotation);
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

        // printf("TYPE: %s VALUE: %s\n", node -> type, node -> content);

        generateIntermidiateRepresentation(child1, symbolTable, 1);

        node -> line = child1 -> line;
        node -> column = child1 -> column;

        if(strcmp(child1 -> annotation, "int") == 0) {
            ;
        } else if(strcmp(child1 -> annotation, "double") == 0) {
            ;
        } else if(strcmp(child1 -> annotation, "boolean") == 0) {
            ;
        } else if(strcmp(child1 -> annotation, "String") == 0) {
            ;
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
            // printf(" ret i32 %c%s\n", '%', child1 -> llvm);
        } else if(strcmp(child1 -> annotation, "double") == 0) {
            // printf(" ret double %c%s\n", '%', child1 -> llvm);
        } else if(strcmp(child1 -> annotation, "boolean") == 0) {
            // printf(" ret i1 %c%s\n", '%', child1 -> llvm);
        }

    } else if(strcmp(node -> type, "MethodDecl") == 0) {
        SymbolTableNode * auxSymbolTable = NULL;

        if(annotationsCurrentMethodNode == NULL)
            auxSymbolTable = symbolTable -> child;
        else {
            auxSymbolTable = annotationsCurrentMethodNode -> next;
        }

        while(auxSymbolTable != NULL) {
            if(auxSymbolTable -> flagMethod) {
                annotationsCurrentMethodNode = auxSymbolTable;
                break;
            }
            auxSymbolTable = auxSymbolTable -> next;
        }

        if(strcmp(node -> stnode -> child -> type, "int") == 0) {
            printf("define i32 @%s.%s", symbolTable -> name, node -> stnode -> name);
        } else if(strcmp(node -> stnode -> child -> type, "double") == 0) {
            printf("define double @%s.%s", symbolTable -> name, node -> stnode -> name);
        } else if(strcmp(node -> stnode -> child -> type, "boolean") == 0) {
            printf("define i1 @%s.%s", symbolTable -> name, node -> stnode -> name);
        } else {
            printf("define void @%s.%s", symbolTable -> name, node -> stnode -> name);
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
                    printf("i8** %c%s", '%', aux -> name);
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

        } else if(strcmp(node -> child -> type, "Double") == 0) {

        } else if(strcmp(node -> child -> type, "Bool") == 0) {

        }

    } else if(strcmp(node -> type, "Program") == 0) {
        printf("define i32 @main(i32 %cargc, i8** %cargv) {\n call void @%s.main(i32 %cargc, i8** %cargv)\n ret i32 0\n}\n", '%', '%', symbolTable -> name, '%', '%');

        generateIntermidiateRepresentation(node -> child, symbolTable, 0);

    } else if(strcmp(node -> type, "MethodBody") == 0) {
        generateIntermidiateRepresentation(node -> child, symbolTable, 0);

        if(strcmp(annotationsCurrentMethodNode -> type, "void") == 0) {
            printf(" ret void\n");
        }

        printf("}\n");

    } else {
        generateIntermidiateRepresentation(node -> child, symbolTable, 0);

    }

    if(flagVariable != 2) {
        generateIntermidiateRepresentation(node -> next, symbolTable, 0);

    }
}

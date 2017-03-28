%{
    #include <string.h>
    #include "tree.h"

    int yylex(void);
    void yyerror(const char *s);

    extern Node *tree;
    Node *auxNode;
    extern char *auxType;
    extern int flagError;
    int flagAux = 0;
%}

%union {
	char *value;
	Node *node;
}

%token BOOL CLASS DO DOTLENGTH DOUBLE ELSE IF INT PARSEINT PRINT PUBLIC RETURN STATIC STRING VOID WHILE
%token <value> BOOLLIT
%token AND OR EQ NEQ LEQ GEQ
%token RESERVED
%token <value> ID
%token <value> DECLIT
%token <value> REALLIT
%token <value> STRLIT

%right '='
%left OR
%left AND
%left EQ NEQ
%left LEQ GEQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%left '!'
%nonassoc IFX
%nonassoc ELSE
%right UPLUS
%right UMINUS



%type <node> Program ProgramContent
%type <node> FieldDecl FieldDeclContent
%type <node> MethodDecl MethodHeader FormalParams FormalParamsContent MethodBody MethodBodyContent VarDecl VarDeclContent
%type <node> Type
%type <node> Statement Statement2 Expr Assignment MethodInvocation MethodInvocationContent ParseArgs

%%

Program:
        CLASS ID '{' ProgramContent '}'                 { if (flagError == 0) { $$ = createNode("Program", NULL, createNode("Id", $2, $4, 's'), 'c'); tree = $$; } }
    ;

ProgramContent:
        FieldDecl ProgramContent                        { if (flagError == 0) { auxNode = $1; while(auxNode->next != NULL) { auxNode = auxNode->next; }; auxNode->next = $2; $$ = $1; } }
    |   MethodDecl ProgramContent                       { if (flagError == 0) { $1->next = $2; $$ = $1; } }
    |   ';' ProgramContent                              { if (flagError == 0) { $$ = $2; } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;

FieldDecl:
        PUBLIC STATIC Type ID FieldDeclContent ';'      { if (flagError == 0) { $3->next = createNode("Id", $4, NULL, 0); $$ = createNode("FieldDecl", NULL, $3, 'c'); $$->next = $5; } }
    |   error ';'                                       { flagError = 1; }
    ;

FieldDeclContent:
        ',' ID FieldDeclContent                         { if (flagError == 0) { $$ = createNode("FieldDecl", NULL, createNode(auxType, NULL, createNode("Id", $2, NULL, 0), 's'), 'c'); $$->next = $3; } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;

MethodDecl:
        PUBLIC STATIC MethodHeader MethodBody           { if (flagError == 0) { $3->next = $4; $$ = createNode("MethodDecl", NULL, $3, 'c'); } }
    ;

MethodHeader:
        Type ID '(' ')'                                 { if (flagError == 0) { $1->next = createNode("Id", $2, createNode("MethodParams", NULL, NULL, 0), 's'); $$ = createNode("MethodHeader", NULL, $1, 'c'); } }
    |   VOID ID '(' ')'                                 { if (flagError == 0) { $$ = createNode("MethodHeader", NULL, createNode("Void", NULL, createNode("Id", $2, createNode("MethodParams", NULL, NULL, 0), 's'), 's'), 'c'); } }
    |   Type ID '(' FormalParams ')'                    { if (flagError == 0) { $1->next = createNode("Id", $2, createNode("MethodParams", NULL, $4, 'c'), 's'); $$ = createNode("MethodHeader", NULL, $1, 'c'); } }
    |   VOID ID '(' FormalParams ')'                    { if (flagError == 0) { $$ = createNode("MethodHeader", NULL, createNode("Void", NULL, createNode("Id", $2, createNode("MethodParams", NULL, $4, 'c'), 's'), 's'), 'c'); } }
    ;

MethodBody:
        '{' MethodBodyContent '}'                       { if (flagError == 0) { $$ = createNode("MethodBody", NULL, $2, 'c'); } }
    ;

MethodBodyContent:
        VarDecl MethodBodyContent                       { if (flagError == 0) { auxNode = $1; while(auxNode->next != NULL) { auxNode = auxNode->next; } auxNode->next = $2; $$ = $1; } }
/*    |   Statement MethodBodyContent                     { if (flagError == 0) { if($1 != NULL) { if(stm2) { stm2 = 0; auxNode = $1; while(auxNode->next != NULL) { auxNode = auxNode->next; } $$ = createNode("Block", NULL, $1, 'c'); $$->next = $2; } else { $1->next = $2; $$ = $1; } } else { $$ = $2; } } }*/
    |   Statement MethodBodyContent                     { if (flagError == 0) { if($1 != NULL) { $1->next = $2; $$ = $1; } else {$$ = $2;} } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;

FormalParams:
        Type ID FormalParamsContent                     { if (flagError == 0) { $1->next = createNode("Id", $2, NULL, 0); $$ = createNode("ParamDecl", NULL, $1, 'c'); $$->next = $3; } }
    |   STRING '[' ']' ID                               { if (flagError == 0) { $$ = createNode("ParamDecl", NULL, createNode("StringArray", NULL, createNode("Id", $4, NULL, 0), 's'), 'c'); } }
    ;

FormalParamsContent:
        ',' Type ID FormalParamsContent                 { if (flagError == 0) { $2->next = createNode("Id", $3, NULL, 0); $$ = createNode("ParamDecl", NULL, $2, 'c'); $$->next = $4; } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;

VarDecl:
        Type ID VarDeclContent ';'                      { if (flagError == 0) { $1->next = createNode("Id", $2, NULL, 0); $$ = createNode("VarDecl", NULL, $1, 'c'); $$->next = $3; } }
    ;

VarDeclContent:
        ',' ID VarDeclContent                           { if (flagError == 0) { $$ = createNode("VarDecl", NULL, createNode(auxType, NULL, createNode("Id", $2, NULL, 0), 's'), 'c'); $$->next = $3; } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;

Type:
        BOOL                                            { if (flagError == 0) { free(auxType); auxType = strdup("Bool"); $$ = createNode("Bool", NULL, NULL, 0); } }
    |   INT                                             { if (flagError == 0) { free(auxType); auxType = strdup("Int"); $$ = createNode("Int", NULL, NULL, 0); } }
    |   DOUBLE                                          { if (flagError == 0) { free(auxType); auxType = strdup("Double"); $$ = createNode("Double", NULL, NULL, 0); } }
    ;

Statement:
        error ';'                                       { flagError = 1; }

    |   '{' Statement2 '}'                              { if (flagError == 0) { if($2 == NULL) { $$ = createNode("Block", NULL, NULL, 0); } else { if($2->next == NULL) { $$ = $2; } else { $$ = createNode("Block", NULL, $2, 'c'); } } } }
/*
    |   '{' Statement2 '}'                              { if (flagError == 0) { stm2 = 1; $$ = $2; } }

    |   IF '(' Expr ')' Statement   %prec IFX           { if (flagError == 0) { stm2 = 0; if($5 != NULL && $5->next == NULL) { $3->next = $5; $5->next = createNode("Block", NULL, NULL, 0); $$ = createNode("If", NULL, $3, 'c'); } else { $3->next = createNode("Block", NULL, $5, 'c'); $3->next->next = createNode("Block", NULL, NULL, 0); $$ = createNode("If", NULL, $3, 'c'); } } }
    |   IF '(' Expr ')' Statement ELSE Statement        { if (flagError == 0) { stm2 = 0; if($5 != NULL && $5->next == NULL) { $3->next = $5; if($7 != NULL && $7->next == NULL) { $5->next = $7; } else { $5->next = createNode("Block", NULL, $7, 'c'); } } else { $3->next = createNode("Block", NULL, $5, 'c'); if($7 != NULL && $7->next == NULL) { $3->next->next = $7; } else { $3->next->next = createNode("Block", NULL, $7, 'c'); } } $$ = createNode("If", NULL, $3, 'c'); } }

    |   WHILE '(' Expr ')' Statement                    { if (flagError == 0) { stm2 = 0; if($5 != NULL && $5->next == NULL) { $3->next = $5; } else { $3->next = createNode("Block", NULL, $5, 'c'); } $$ = createNode("While", NULL, $3, 'c'); } }
    |   DO Statement WHILE '(' Expr ')' ';'             { if (flagError == 0) { stm2 = 0; if($2 != NULL && $2->next == NULL) { $2->next = $5; $$ = createNode("DoWhile", NULL, $2, 'c'); } else { $$ = createNode("DoWhile", NULL, createNode("Block", NULL, $2, 'c'), 'c'); $$->child->next = $5; } } }
*/
    |   IF '(' Expr ')' Statement   %prec IFX           { if (flagError == 0) { if($5 != NULL) { $3->next = $5; $5->next = createNode("Block", NULL, NULL, 0); } else { $3->next = createNode("Block", NULL, createNode("Block", NULL, NULL, 0), 's'); } $$ = createNode("If", NULL, $3, 'c'); } }
    |   IF '(' Expr ')' Statement ELSE Statement        { if (flagError == 0) { if($5 != NULL) { if($7 != NULL) { $3->next = $5; $5->next = $7; } else { $3->next = $5; $5->next = createNode("Block", NULL, NULL, 0); } } else { if ($7 != NULL) { $3->next = createNode("Block", NULL, $7, 's'); } else { $3->next = createNode("Block", NULL, createNode("Block", NULL, NULL, 0), 's'); } } $$ = createNode("If", NULL, $3, 'c'); } }

    |   WHILE '(' Expr ')' Statement                    { if (flagError == 0) { if($5 != NULL) { $3->next = $5; } else { $3->next = createNode("Block", NULL, NULL, 0); } $$ = createNode("While", NULL, $3, 'c'); } }
    |   DO Statement WHILE '(' Expr ')' ';'             { if (flagError == 0) { if($2 != NULL) { $2->next = $5; $$ = createNode("DoWhile", NULL, $2, 'c'); } else { $$ = createNode("DoWhile", NULL, createNode("Block", NULL, $5, 's'), 'c'); } } }

    |   PRINT '(' Expr ')' ';'                          { if (flagError == 0) { $$ = createNode("Print", NULL, $3, 'c'); } }
    |   PRINT '(' STRLIT ')' ';'                        { if (flagError == 0) { $$ = createNode("Print", NULL, createNode("StrLit", $3, NULL, 0), 'c'); } }

    |   ';'                                             { if (flagError == 0) { $$ = NULL; } }
    |   Assignment ';'                                  { if (flagError == 0) { $$ = $1; } }
    |   MethodInvocation ';'                            { if (flagError == 0) { $$ = $1; } }
    |   ParseArgs ';'                                   { if (flagError == 0) { $$ = $1; } }

    |   RETURN ';'                                      { if (flagError == 0) { $$ = createNode("Return", NULL, NULL, 0); } }
    |   RETURN Expr ';'                                 { if (flagError == 0) { $$ = createNode("Return", NULL, $2, 'c'); } }
    ;

Statement2:
/*        Statement Statement2                            { if (flagError == 0) { if($1 != NULL) { if($2 != NULL) { if(strcmp($2->type, "Block") != 0) { $1->next = $2; $$ = createNode("Block", NULL, $1, 'c'); } else { $1->next = $2->next; $2->next = $1; $$ = $2; } } else { $$ = $1; } } else {$$ = $2;} } }*/
        Statement Statement2                            { if (flagError == 0) { if($1 != NULL) { $1->next = $2; $$ = $1; } else {$$ = $2;} } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;
/*
Statement2:
        Statement Statement2                            { if (flagError == 0) { if($1 != NULL) { $1->next = $2; $$ = $1; } else {$$ = $2;} } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;
*/
Assignment:
        ID '=' Expr                                     { if (flagError == 0) { $$ = createNode("Assign", NULL, createNode("Id", $1, $3, 's'), 'c'); } }
    ;

MethodInvocation:
        ID '(' ')'                                      { if (flagError == 0) { $$ = createNode("Call", NULL, createNode("Id", $1, NULL, 0), 'c'); } }
    |   ID '(' Expr MethodInvocationContent ')'         { if (flagError == 0) { $3->next = $4; $$ = createNode("Call", NULL, createNode("Id", $1, $3, 's'), 'c'); } }
    |   ID '(' error ')'                                { flagError = 1; }
    ;

MethodInvocationContent:
        ',' Expr MethodInvocationContent                { if (flagError == 0) { $2->next = $3; $$ = $2; } }
    |   %empty                                          { if (flagError == 0) { $$ = NULL; } }
    ;

ParseArgs:
        PARSEINT '(' ID '[' Expr ']' ')'                { if (flagError == 0) { $$ = createNode("ParseArgs", NULL, createNode("Id", $3, $5, 's'), 'c'); } }
    |   PARSEINT '(' error ')'                          { flagError = 1; }
    ;

Expr:
        '(' error ')'                                   { flagError = 1; }

    |   Assignment                                      { if (flagError == 0) { $$ = $1; } }
    |   MethodInvocation                                { if (flagError == 0) { $$ = $1; } }
    |   ParseArgs                                       { if (flagError == 0) { $$ = $1; } }

    |   Expr AND Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("And", NULL, $1, 'c'); } }
    |   Expr OR Expr                                    { if (flagError == 0) { $1->next = $3; $$ = createNode("Or", NULL, $1, 'c'); } }

    |   Expr EQ Expr                                    { if (flagError == 0) { $1->next = $3; $$ = createNode("Eq", NULL, $1, 'c'); } }
    |   Expr GEQ Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Geq", NULL, $1, 'c'); } }
    |   Expr '>' Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Gt", NULL, $1, 'c'); } }
    |   Expr LEQ Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Leq", NULL, $1, 'c'); } }
    |   Expr '<' Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Lt", NULL, $1, 'c'); } }
    |   Expr NEQ Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Neq", NULL, $1, 'c'); } }

    |   Expr '+' Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Add", NULL, $1, 'c'); } }
    |   Expr '-' Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Sub", NULL, $1, 'c'); } }
    |   Expr '*' Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Mul", NULL, $1, 'c'); } }
    |   Expr '/' Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Div", NULL, $1, 'c'); } }
    |   Expr '%' Expr                                   { if (flagError == 0) { $1->next = $3; $$ = createNode("Mod", NULL, $1, 'c'); } }

    |   '+' Expr        %prec UPLUS                     { if (flagError == 0) { $$ = createNode("Plus", NULL, $2, 'c'); } }
    |   '-' Expr        %prec UMINUS                    { if (flagError == 0) { $$ = createNode("Minus", NULL, $2, 'c'); } }
    |   '!' Expr                                        { if (flagError == 0) { $$ = createNode("Not", NULL, $2, 'c'); } }

    |   ID                                              { if (flagError == 0) { $$ = createNode("Id", $1, NULL, 0); } }
    |   ID DOTLENGTH                                    { if (flagError == 0) { $$ = createNode("Length", NULL, createNode("Id", $1, NULL, 0), 'c'); } }

    |   '(' Expr ')'                                    { if (flagError == 0) { $$ = $2; } }

    |   BOOLLIT                                         { if (flagError == 0) { $$ = createNode("BoolLit", $1, NULL, 0); } }
    |   DECLIT                                          { if (flagError == 0) { $$ = createNode("DecLit", $1, NULL, 0); } }
    |   REALLIT                                         { if (flagError == 0) { $$ = createNode("RealLit", $1, NULL, 0); } }
    ;

%%

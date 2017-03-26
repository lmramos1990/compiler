%{
    #include <string.h>
    #include "tree.h"

    int yylex(void);
    void yyerror(const char *s);

    extern Node *tree;
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

%left '=' NEQ
%left AND OR EQ
%left LEQ GEQ '>' '<'
%left '+' '-'
%left '*' '/' '%' '!'
%nonassoc IFX
%nonassoc ELSE



%type <node> Program ProgramContent
%type <node> FieldDecl FieldDeclContent
%type <node> MethodDecl MethodHeader FormalParams FormalParamsContent MethodBody MethodBodyContent VarDecl VarDeclContent
%type <node> Type
%type <node> Statement Statement2 Expr Assignment MethodInvocation MethodInvocationContent ParseArgs

%%

Program:
        CLASS ID '{' ProgramContent '}'                 { $$ = createNode("Program", NULL, createNode("Id", $2, $4, 's'), 'c'); tree = $$; }
    ;

ProgramContent:
        FieldDecl ProgramContent                        { $1->next = $2; $$ = $1; }
    |   MethodDecl ProgramContent                       { $1->next = $2; $$ = $1; }
    |   ';' ProgramContent                              { $$ = $2; }
    |   %empty                                          { $$ = NULL; }
    ;

FieldDecl:
        PUBLIC STATIC Type ID FieldDeclContent ';'      { $3->next = createNode("Id", $4, $5, 's'); $$ = createNode("FieldDecl", NULL, $3, 'c'); }
    |   error ';'
    ;

FieldDeclContent:
        ',' ID FieldDeclContent                         { $$ = createNode("Id", $2, $3, 's'); }
    |   %empty                                          { $$ = NULL; }
    ;

MethodDecl:
        PUBLIC STATIC MethodHeader MethodBody           { $3->next = $4; $$ = createNode("MethodDecl", NULL, $3, 'c'); }
    ;

MethodHeader:
        Type ID '(' ')'                                 { $1->next = createNode("Id", $2, NULL, 0); $$ = createNode("MethodHeader", NULL, $1, 'c'); }
    |   VOID ID '(' ')'                                 { $$ = createNode("MethodHeader", NULL, createNode("Void", NULL, createNode("Id", $2, NULL, 0), 's'), 'c'); }
    |   Type ID '(' FormalParams ')'                    { $1->next = createNode("Id", $2, $4, 's'); $$ = createNode("MethodHeader", NULL, $1, 'c'); }
    |   VOID ID '(' FormalParams ')'                    { $$ = createNode("MethodHeader", NULL, createNode("Void", NULL, createNode("Id", $2, createNode("MethodParams", NULL, $4, 'c'), 's'), 's'), 'c'); }
    ;

MethodBody:
        '{' MethodBodyContent '}'                       { $$ = createNode("MethodBody", NULL, $2, 'c'); }
    ;

MethodBodyContent:
        VarDecl MethodBodyContent                       { $1->next = $2; $$ = $1; }
    |   Statement MethodBodyContent                     { $1->next = $2; $$ = $1; }
    |   %empty                                          { $$ = NULL; }
    ;

FormalParams:
        Type ID FormalParamsContent                     { $1->next = createNode("Id", $2, $3, 's'); $$ = createNode("ParamDecl", NULL, $1, 'c'); }
    |   STRING '[' ']' ID                               { $$ = createNode("ParamDecl", NULL, createNode("StringArray", NULL, createNode("Id", $4, NULL, 0), 's'), 'c'); }
    ;

FormalParamsContent:
        ',' Type ID FormalParamsContent                 { $2->next = createNode("Id", $3, $4, 's'); $$ = $2; }
    |   %empty                                          { $$ = NULL; }
    ;

VarDecl:
        Type ID VarDeclContent ';'                      { $1->next = createNode("Id", $2, $3, 's'); $$ = createNode("VarDecl", NULL, $1, 'c'); }
    ;

VarDeclContent:
        ',' ID VarDeclContent                           { $$ = createNode("Id", $2, $3, 's'); }
    |   %empty                                          { $$ = NULL; }
    ;

Type:
        BOOL                                            { $$ = createNode("Bool", NULL, NULL, 0); }
    |   INT                                             { $$ = createNode("Int", NULL, NULL, 0); }
    |   DOUBLE                                          { $$ = createNode("Double", NULL, NULL, 0); }
    ;

Statement:
        error ';'

    |   '{' Statement2 '}'                              { $$ = createNode("Block", NULL, $2, 'c'); }

    |   IF '(' Expr ')' Statement   %prec IFX           { $3->next = $5; $$ = createNode("If", NULL, $3, 'c'); }
    |   IF '(' Expr ')' Statement ELSE Statement        { $3->next = $5; $5->next = $7; $$ = createNode("If", NULL, $3, 'c'); }

    |   WHILE '(' Expr ')' Statement                    { $3->next = $5; $$ = createNode("While", NULL, $3, 'c'); }
    |   DO Statement WHILE '(' Expr ')' ';'             { $2->next = $5; $$ = createNode("DoWhile", NULL, $2, 'c'); }

    |   PRINT '(' Expr ')' ';'                          { $$ = createNode("Print", NULL, $3, 'c'); }
    |   PRINT '(' STRLIT ')' ';'                        { $$ = createNode("Print", NULL, createNode("StrLit", $3, NULL, 0), 'c'); }

    |   ';'                                             { $$ = NULL; }
    |   Assignment ';'                                  { $$ = $1; }
    |   MethodInvocation ';'                            { $$ = $1; }
    |   ParseArgs ';'                                   { $$ = $1; }

    |   RETURN ';'                                      { $$ = createNode("Return", NULL, NULL, 0); }
    |   RETURN Expr ';'                                 { $$ = createNode("Return", NULL, $2, 'c'); }
    ;

Statement2:
        Statement Statement2                            { $1->next = $2; $$ = $1; }
    |   %empty                                          { $$ = NULL; }
    ;

Assignment:
        ID '=' Expr                                     { $$ = createNode("Assign", NULL, createNode("Id", $1, $3, 's'), 'c'); }
    ;

MethodInvocation:
        ID '(' ')'                                      { $$ = createNode("Call", NULL, createNode("Id", $1, NULL, 0), 'c'); }
    |   ID '(' Expr MethodInvocationContent ')'         { $3->next = $4; $$ = createNode("Call", NULL, createNode("Id", $1, $3, 's'), 'c'); }
    |   ID '(' error ')'
    ;

MethodInvocationContent:
        ',' Expr MethodInvocationContent                { $2->next = $3; $$ = $2; }
    |   %empty                                          { $$ = NULL; }
    ;

ParseArgs:
        PARSEINT '(' ID '[' Expr ']' ')'                { $$ = createNode("ParseArgs", NULL, createNode("Id", $3, $5, 's'), 'c'); }
    |   PARSEINT '(' error ')'
    ;

Expr:
        '(' error ')'

    |   Assignment                                      { $$ = $1; }
    |   MethodInvocation                                { $$ = $1; }
    |   ParseArgs                                       { $$ = $1; }

    |   Expr AND Expr                                   { $1->next = $3; $$ = createNode("And", NULL, $1, 'c'); }
    |   Expr OR Expr                                    { $1->next = $3; $$ = createNode("Or", NULL, $1, 'c'); }

    |   Expr EQ Expr                                    { $1->next = $3; $$ = createNode("Eq", NULL, $1, 'c'); }
    |   Expr GEQ Expr                                   { $1->next = $3; $$ = createNode("Geq", NULL, $1, 'c'); }
    |   Expr '>' Expr                                   { $1->next = $3; $$ = createNode("Gt", NULL, $1, 'c'); }
    |   Expr LEQ Expr                                   { $1->next = $3; $$ = createNode("Leq", NULL, $1, 'c'); }
    |   Expr '<' Expr                                   { $1->next = $3; $$ = createNode("Lt", NULL, $1, 'c'); }
    |   Expr NEQ Expr                                   { $1->next = $3; $$ = createNode("Neq", NULL, $1, 'c'); }

    |   Expr '+' Expr                                   { $1->next = $3; $$ = createNode("Add", NULL, $1, 'c'); }
    |   Expr '-' Expr                                   { $1->next = $3; $$ = createNode("Sub", NULL, $1, 'c'); }
    |   Expr '*' Expr                                   { $1->next = $3; $$ = createNode("Mul", NULL, $1, 'c'); }
    |   Expr '/' Expr                                   { $1->next = $3; $$ = createNode("Div", NULL, $1, 'c'); }
    |   Expr '%' Expr                                   { $1->next = $3; $$ = createNode("Mod", NULL, $1, 'c'); }

    |   '+' Expr                                        { $$ = createNode("Plus", NULL, $2, 'c'); }
    |   '-' Expr                                        { $$ = createNode("Minus", NULL, $2, 'c'); }
    |   '!' Expr                                        { $$ = createNode("Not", NULL, $2, 'c'); }

    |   ID                                              { $$ = createNode("Id", $1, NULL, 0); }
    |   ID DOTLENGTH                                    { $$ = createNode("Length", NULL, createNode("Id", $1, NULL, 0), 'c'); }

    |   '(' Expr ')'                                    { $$ = $2; }

    |   BOOLLIT                                         { $$ = createNode("BoolLit", $1, NULL, 0); }
    |   DECLIT                                          { $$ = createNode("DecLit", $1, NULL, 0); }
    |   REALLIT                                         { $$ = createNode("RealLit", $1, NULL, 0); }
    ;

%%

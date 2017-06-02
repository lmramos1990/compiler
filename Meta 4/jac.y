%{
    #include <string.h>
    #include "structs.h"

    int yylex(void);
    void yyerror(const char *s);
    void destroyPayloadNode(Payload* pl);

    extern ASTNode * tree;
    ASTNode * auxNode;
    extern char * auxType;
    extern int flagError;
    int flagAux = 0;
%}

%union {
	Payload * value;
	ASTNode * node;
}

%token BOOL CLASS DO DOUBLE ELSE IF INT PRINT PUBLIC STATIC STRING VOID WHILE
%token <value> RETURN
%token <value> PARSEINT
%token <value> DOTLENGTH
%token <value> BOOLLIT
%token <value> AND
%token <value> OR
%token <value> EQ
%token <value> NEQ
%token <value> LEQ
%token <value> GEQ
%token RESERVED
%token <value> ID
%token <value> DECLIT
%token <value> REALLIT
%token <value> STRLIT
%token <value> '='
%token <value> '<'
%token <value> '>'
%token <value> '*'
%token <value> '/'
%token <value> '+'
%token <value> '-'
%token <value> '%'
%token <value> '!'

%right '='
%left OR
%left AND
%left EQ NEQ
%left LEQ GEQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%right '!' UMINUS UPLUS
%nonassoc IFX
%nonassoc ELSE

%type <node> ProgramContent
%type <node> FieldDecl FieldDeclContent
%type <node> MethodDecl MethodHeader FormalParams FormalParamsContent MethodBody MethodBodyContent VarDecl VarDeclContent
%type <node> Type
%type <node> Statement Statement2 Expr Expr2 Assignment MethodInvocation MethodInvocationContent ParseArgs

%destructor { destroyPayloadNode($$); } <value>
%destructor { destroyAST($$); } <node>

%%

Program:
        CLASS ID '{' ProgramContent '}'                 { tree = createASTNode("Program", NULL, createASTNode("Id", $2, $4, 's'), 'c'); }
    ;

ProgramContent:
        FieldDecl ProgramContent                        { auxNode = $1; while(auxNode->next != NULL) { auxNode = auxNode->next; }; auxNode->next = $2; $$ = $1; }
    |   MethodDecl ProgramContent                       { $1->next = $2; $$ = $1; }
    |   ';' ProgramContent                              { $$ = $2; }
    |   %empty                                          { $$ = NULL; }
    ;

FieldDecl:
        PUBLIC STATIC Type ID FieldDeclContent ';'      { $3->next = createASTNode("Id", $4, NULL, 0); $$ = createASTNode("FieldDecl", NULL, $3, 'c'); $$->next = $5; }
    |   error ';'                                       { $$ = createASTNode("Dummy", NULL, NULL, 0); }
    ;

FieldDeclContent:
        ',' ID FieldDeclContent                         { $$ = createASTNode("FieldDecl", NULL, createASTNode(auxType, NULL, createASTNode("Id", $2, NULL, 0), 's'), 'c'); $$->next = $3; }
    |   %empty                                          { $$ = NULL; }
    ;

MethodDecl:
        PUBLIC STATIC MethodHeader MethodBody           { $3->next = $4; $$ = createASTNode("MethodDecl", NULL, $3, 'c'); }
    ;

MethodHeader:
        Type ID '(' ')'                                 { $1->next = createASTNode("Id", $2, createASTNode("MethodParams", NULL, NULL, 0), 's'); $$ = createASTNode("MethodHeader", NULL, $1, 'c'); }
    |   VOID ID '(' ')'                                 { $$ = createASTNode("MethodHeader", NULL, createASTNode("Void", NULL, createASTNode("Id", $2, createASTNode("MethodParams", NULL, NULL, 0), 's'), 's'), 'c'); }
    |   Type ID '(' FormalParams ')'                    { $1->next = createASTNode("Id", $2, createASTNode("MethodParams", NULL, $4, 'c'), 's'); $$ = createASTNode("MethodHeader", NULL, $1, 'c'); }
    |   VOID ID '(' FormalParams ')'                    { $$ = createASTNode("MethodHeader", NULL, createASTNode("Void", NULL, createASTNode("Id", $2, createASTNode("MethodParams", NULL, $4, 'c'), 's'), 's'), 'c'); }
    ;

MethodBody:
        '{' MethodBodyContent '}'                       { $$ = createASTNode("MethodBody", NULL, $2, 'c'); }
    ;

MethodBodyContent:
        VarDecl MethodBodyContent                       { auxNode = $1; while(auxNode->next != NULL) { auxNode = auxNode->next; } auxNode->next = $2; $$ = $1; }
    |   Statement MethodBodyContent                     { if($1 != NULL) { $1->next = $2; $$ = $1; } else {$$ = $2;} }
    |   %empty                                          { $$ = NULL; }
    ;

FormalParams:
        Type ID FormalParamsContent                     { $1->next = createASTNode("Id", $2, NULL, 0); $$ = createASTNode("ParamDecl", NULL, $1, 'c'); $$->next = $3; }
    |   STRING '[' ']' ID                               { $$ = createASTNode("ParamDecl", NULL, createASTNode("StringArray", NULL, createASTNode("Id", $4, NULL, 0), 's'), 'c'); }
    ;

FormalParamsContent:
        ',' Type ID FormalParamsContent                 { $2->next = createASTNode("Id", $3, NULL, 0); $$ = createASTNode("ParamDecl", NULL, $2, 'c'); $$->next = $4; }
    |   %empty                                          { $$ = NULL; }
    ;

VarDecl:
        Type ID VarDeclContent ';'                      { $1->next = createASTNode("Id", $2, NULL, 0); $$ = createASTNode("VarDecl", NULL, $1, 'c'); $$->next = $3; }
    ;

VarDeclContent:
        ',' ID VarDeclContent                           { $$ = createASTNode("VarDecl", NULL, createASTNode(auxType, NULL, createASTNode("Id", $2, NULL, 0), 's'), 'c'); $$->next = $3; }
    |   %empty                                          { $$ = NULL; }
    ;

Type:
        BOOL                                            { free(auxType); auxType = strdup("Bool"); $$ = createASTNode("Bool", NULL, NULL, 0); }
    |   INT                                             { free(auxType); auxType = strdup("Int"); $$ = createASTNode("Int", NULL, NULL, 0); }
    |   DOUBLE                                          { free(auxType); auxType = strdup("Double"); $$ = createASTNode("Double", NULL, NULL, 0); }
    ;

Statement:
        error ';'                                       { $$ = createASTNode("Dummy", NULL, NULL, 0); }

    |   '{' Statement2 '}'                              { if($2 == NULL) { $$ = $2; } else { if($2->next == NULL) { $$ = $2; } else { $$ = createASTNode("Block", NULL, $2, 'c'); } } }

    |   IF '(' Expr2 ')' Statement   %prec IFX          { if($5 != NULL) { $3->next = $5; $5->next = createASTNode("Block", NULL, NULL, 0); } else { $3->next = createASTNode("Block", NULL, createASTNode("Block", NULL, NULL, 0), 's'); } $$ = createASTNode("If", NULL, $3, 'c'); }
    |   IF '(' Expr2 ')' Statement ELSE Statement       { if($5 != NULL) { if($7 != NULL) { $3->next = $5; $5->next = $7; } else { $3->next = $5; $5->next = createASTNode("Block", NULL, NULL, 0); } } else { if ($7 != NULL) { $3->next = createASTNode("Block", NULL, $7, 's'); } else { $3->next = createASTNode("Block", NULL, createASTNode("Block", NULL, NULL, 0), 's'); } } $$ = createASTNode("If", NULL, $3, 'c'); }

    |   WHILE '(' Expr2 ')' Statement                   { if($5 != NULL) { $3->next = $5; } else { $3->next = createASTNode("Block", NULL, NULL, 0); } $$ = createASTNode("While", NULL, $3, 'c'); }
    |   DO Statement WHILE '(' Expr2 ')' ';'            { if($2 != NULL) { $2->next = $5; $$ = createASTNode("DoWhile", NULL, $2, 'c'); } else { $$ = createASTNode("DoWhile", NULL, createASTNode("Block", NULL, $5, 's'), 'c'); } }

    |   PRINT '(' Expr2 ')' ';'                         { $$ = createASTNode("Print", NULL, $3, 'c'); }
    |   PRINT '(' STRLIT ')' ';'                        { $$ = createASTNode("Print", NULL, createASTNode("StrLit", $3, NULL, 0), 'c'); }

    |   ';'                                             { $$ = NULL; }
    |   Assignment ';'                                  { $$ = $1; }
    |   MethodInvocation ';'                            { $$ = $1; }
    |   ParseArgs ';'                                   { $$ = $1; }

    |   RETURN ';'                                      { $$ = createASTNode("Return", $1, NULL, 0); }
    |   RETURN Expr2 ';'                                { $$ = createASTNode("Return", $1, $2, 'c'); }
    ;

Statement2:
        Statement Statement2                            { if($1 != NULL) { $1->next = $2; $$ = $1; } else {$$ = $2;} }
    |   %empty                                          { $$ = NULL; }
    ;

Assignment:
        ID '=' Expr2                                    { $$ = createASTNode("Assign", $2, createASTNode("Id", $1, $3, 's'), 'c'); }
    ;

MethodInvocation:
        ID '(' ')'                                      { $$ = createASTNode("Call", NULL, createASTNode("Id", $1, NULL, 0), 'c'); }
    |   ID '(' Expr2 MethodInvocationContent ')'        { $3->next = $4; $$ = createASTNode("Call", NULL, createASTNode("Id", $1, $3, 's'), 'c'); }
    |   ID '(' error ')'                                { $$ = createASTNode("Dummy", $1, NULL, 0); }
    ;

MethodInvocationContent:
        ',' Expr2 MethodInvocationContent               { $2->next = $3; $$ = $2; }
    |   %empty                                          { $$ = NULL; }
    ;

ParseArgs:
        PARSEINT '(' ID '[' Expr2 ']' ')'               { $$ = createASTNode("ParseArgs", $1, createASTNode("Id", $3, $5, 's'), 'c'); }
    |   PARSEINT '(' error ')'                          { $$ = createASTNode("Dummy", $1, NULL, 0); }
    ;

Expr2:
        Assignment                                      { $$ = $1; }
    |   Expr                                            { $$ = $1; }
    ;

Expr:
        '(' error ')'                                   { $$ = createASTNode("Dummy", NULL, NULL, 0); }

    |   MethodInvocation                                { $$ = $1; }
    |   ParseArgs                                       { $$ = $1; }

    |   Expr AND Expr                                   { $1->next = $3; $$ = createASTNode("And", $2, $1, 'c'); }
    |   Expr OR Expr                                    { $1->next = $3; $$ = createASTNode("Or", $2, $1, 'c'); }

    |   Expr EQ Expr                                    { $1->next = $3; $$ = createASTNode("Eq", $2, $1, 'c'); }
    |   Expr GEQ Expr                                   { $1->next = $3; $$ = createASTNode("Geq", $2, $1, 'c'); }
    |   Expr '>' Expr                                   { $1->next = $3; $$ = createASTNode("Gt", $2, $1, 'c'); }
    |   Expr LEQ Expr                                   { $1->next = $3; $$ = createASTNode("Leq", $2, $1, 'c'); }
    |   Expr '<' Expr                                   { $1->next = $3; $$ = createASTNode("Lt", $2, $1, 'c'); }
    |   Expr NEQ Expr                                   { $1->next = $3; $$ = createASTNode("Neq", $2, $1, 'c'); }

    |   Expr '+' Expr                                   { $1->next = $3; $$ = createASTNode("Add", $2, $1, 'c'); }
    |   Expr '-' Expr                                   { $1->next = $3; $$ = createASTNode("Sub", $2, $1, 'c'); }
    |   Expr '*' Expr                                   { $1->next = $3; $$ = createASTNode("Mul", $2, $1, 'c'); }
    |   Expr '/' Expr                                   { $1->next = $3; $$ = createASTNode("Div", $2, $1, 'c'); }
    |   Expr '%' Expr                                   { $1->next = $3; $$ = createASTNode("Mod", $2, $1, 'c'); }

    |   '+' Expr        %prec UPLUS                     { $$ = createASTNode("Plus", $1, $2, 'c'); }
    |   '-' Expr        %prec UMINUS                    { $$ = createASTNode("Minus", $1, $2, 'c'); }
    |   '!' Expr                                        { $$ = createASTNode("Not", $1, $2, 'c'); }

    |   ID                                              { $$ = createASTNode("Id", $1, NULL, 0); }
    |   ID DOTLENGTH                                    { $$ = createASTNode("Length", $2, createASTNode("Id", $1, NULL, 0), 'c'); }

    |   '(' Expr2 ')'                                   { $$ = $2; }

    |   BOOLLIT                                         { $$ = createASTNode("BoolLit", $1, NULL, 0); }
    |   DECLIT                                          { $$ = createASTNode("DecLit", $1, NULL, 0); }
    |   REALLIT                                         { $$ = createASTNode("RealLit", $1, NULL, 0); }
    ;

%%

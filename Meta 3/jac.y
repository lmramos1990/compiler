%{
    #include <string.h>
    #include "structs.h"

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
%right '!' UMINUS UPLUS
%nonassoc IFX
%nonassoc ELSE

%type <node> Program ProgramContent
%type <node> FieldDecl FieldDeclContent
%type <node> MethodDecl MethodHeader FormalParams FormalParamsContent MethodBody MethodBodyContent VarDecl VarDeclContent
%type <node> Type
%type <node> Statement Statement2 Expr Expr2 Assignment MethodInvocation MethodInvocationContent ParseArgs

%%

Program:
        CLASS ID '{' ProgramContent '}'                 {  $$ = createNode("Program", NULL, createNode("Id", $2, $4, 's'), 'c'); tree = $$; }
    ;

ProgramContent:
        FieldDecl ProgramContent                        {  auxNode = $1; while(auxNode->next != NULL) { auxNode = auxNode->next; }; auxNode->next = $2; $$ = $1; }
    |   MethodDecl ProgramContent                       {  $1->next = $2; $$ = $1; }
    |   ';' ProgramContent                              {  $$ = $2; }
    |   %empty                                          {  $$ = NULL; }
    ;

FieldDecl:
        PUBLIC STATIC Type ID FieldDeclContent ';'      {  $3->next = createNode("Id", $4, NULL, 0); $$ = createNode("FieldDecl", NULL, $3, 'c'); $$->next = $5; }
    |   error ';'                                       { flagError = 1; $$ = createNode("Dummy", NULL, NULL, 0); }
    ;

FieldDeclContent:
        ',' ID FieldDeclContent                         {  $$ = createNode("FieldDecl", NULL, createNode(auxType, NULL, createNode("Id", $2, NULL, 0), 's'), 'c'); $$->next = $3; }
    |   %empty                                          {  $$ = NULL; }
    ;

MethodDecl:
        PUBLIC STATIC MethodHeader MethodBody           {  $3->next = $4; $$ = createNode("MethodDecl", NULL, $3, 'c'); }
    ;

MethodHeader:
        Type ID '(' ')'                                 {  $1->next = createNode("Id", $2, createNode("MethodParams", NULL, NULL, 0), 's'); $$ = createNode("MethodHeader", NULL, $1, 'c'); }
    |   VOID ID '(' ')'                                 {  $$ = createNode("MethodHeader", NULL, createNode("Void", NULL, createNode("Id", $2, createNode("MethodParams", NULL, NULL, 0), 's'), 's'), 'c'); }
    |   Type ID '(' FormalParams ')'                    {  $1->next = createNode("Id", $2, createNode("MethodParams", NULL, $4, 'c'), 's'); $$ = createNode("MethodHeader", NULL, $1, 'c'); }
    |   VOID ID '(' FormalParams ')'                    {  $$ = createNode("MethodHeader", NULL, createNode("Void", NULL, createNode("Id", $2, createNode("MethodParams", NULL, $4, 'c'), 's'), 's'), 'c'); }
    ;

MethodBody:
        '{' MethodBodyContent '}'                       {  $$ = createNode("MethodBody", NULL, $2, 'c'); }
    ;

MethodBodyContent:
        VarDecl MethodBodyContent                       {  auxNode = $1; while(auxNode->next != NULL) { auxNode = auxNode->next; } auxNode->next = $2; $$ = $1; }
    |   Statement MethodBodyContent                     {  if($1 != NULL) { $1->next = $2; $$ = $1; } else {$$ = $2;} }
    |   %empty                                          {  $$ = NULL; }
    ;

FormalParams:
        Type ID FormalParamsContent                     {  $1->next = createNode("Id", $2, NULL, 0); $$ = createNode("ParamDecl", NULL, $1, 'c'); $$->next = $3; }
    |   STRING '[' ']' ID                               {  $$ = createNode("ParamDecl", NULL, createNode("StringArray", NULL, createNode("Id", $4, NULL, 0), 's'), 'c'); }
    ;

FormalParamsContent:
        ',' Type ID FormalParamsContent                 {  $2->next = createNode("Id", $3, NULL, 0); $$ = createNode("ParamDecl", NULL, $2, 'c'); $$->next = $4; }
    |   %empty                                          {  $$ = NULL; }
    ;

VarDecl:
        Type ID VarDeclContent ';'                      {  $1->next = createNode("Id", $2, NULL, 0); $$ = createNode("VarDecl", NULL, $1, 'c'); $$->next = $3; }
    ;

VarDeclContent:
        ',' ID VarDeclContent                           {  $$ = createNode("VarDecl", NULL, createNode(auxType, NULL, createNode("Id", $2, NULL, 0), 's'), 'c'); $$->next = $3; }
    |   %empty                                          {  $$ = NULL; }
    ;

Type:
        BOOL                                            {  free(auxType); auxType = strdup("Bool"); $$ = createNode("Bool", NULL, NULL, 0); }
    |   INT                                             {  free(auxType); auxType = strdup("Int"); $$ = createNode("Int", NULL, NULL, 0); }
    |   DOUBLE                                          {  free(auxType); auxType = strdup("Double"); $$ = createNode("Double", NULL, NULL, 0); }
    ;

Statement:
        error ';'                                       { flagError = 1; $$ = createNode("Dummy", NULL, NULL, 0); }

    |   '{' Statement2 '}'                              {  if($2 == NULL) { $$ = $2; } else { if($2->next == NULL) { $$ = $2; } else { $$ = createNode("Block", NULL, $2, 'c'); } } }

    |   IF '(' Expr2 ')' Statement   %prec IFX          {  if($5 != NULL) { $3->next = $5; $5->next = createNode("Block", NULL, NULL, 0); } else { $3->next = createNode("Block", NULL, createNode("Block", NULL, NULL, 0), 's'); } $$ = createNode("If", NULL, $3, 'c'); }
    |   IF '(' Expr2 ')' Statement ELSE Statement       {  if($5 != NULL) { if($7 != NULL) { $3->next = $5; $5->next = $7; } else { $3->next = $5; $5->next = createNode("Block", NULL, NULL, 0); } } else { if ($7 != NULL) { $3->next = createNode("Block", NULL, $7, 's'); } else { $3->next = createNode("Block", NULL, createNode("Block", NULL, NULL, 0), 's'); } } $$ = createNode("If", NULL, $3, 'c'); }

    |   WHILE '(' Expr2 ')' Statement                   {  if($5 != NULL) { $3->next = $5; } else { $3->next = createNode("Block", NULL, NULL, 0); } $$ = createNode("While", NULL, $3, 'c'); }
    |   DO Statement WHILE '(' Expr2 ')' ';'            {  if($2 != NULL) { $2->next = $5; $$ = createNode("DoWhile", NULL, $2, 'c'); } else { $$ = createNode("DoWhile", NULL, createNode("Block", NULL, $5, 's'), 'c'); } }

    |   PRINT '(' Expr2 ')' ';'                         {  $$ = createNode("Print", NULL, $3, 'c'); }
    |   PRINT '(' STRLIT ')' ';'                        {  $$ = createNode("Print", NULL, createNode("StrLit", $3, NULL, 0), 'c'); }

    |   ';'                                             {  $$ = NULL; }
    |   Assignment ';'                                  {  $$ = $1; }
    |   MethodInvocation ';'                            {  $$ = $1; }
    |   ParseArgs ';'                                   {  $$ = $1; }

    |   RETURN ';'                                      {  $$ = createNode("Return", NULL, NULL, 0); }
    |   RETURN Expr2 ';'                                {  $$ = createNode("Return", NULL, $2, 'c'); }
    ;

Statement2:
        Statement Statement2                            {  if($1 != NULL) { $1->next = $2; $$ = $1; } else {$$ = $2;} }
    |   %empty                                          {  $$ = NULL; }
    ;

Assignment:
        ID '=' Expr2                                    {  $$ = createNode("Assign", NULL, createNode("Id", $1, $3, 's'), 'c'); }
    ;

MethodInvocation:
        ID '(' ')'                                      {  $$ = createNode("Call", NULL, createNode("Id", $1, NULL, 0), 'c'); }
    |   ID '(' Expr2 MethodInvocationContent ')'        {  $3->next = $4; $$ = createNode("Call", NULL, createNode("Id", $1, $3, 's'), 'c'); }
    |   ID '(' error ')'                                { flagError = 1; $$ = createNode("Dummy", NULL, NULL, 0); }
    ;

MethodInvocationContent:
        ',' Expr2 MethodInvocationContent               {  $2->next = $3; $$ = $2; }
    |   %empty                                          {  $$ = NULL; }
    ;

ParseArgs:
        PARSEINT '(' ID '[' Expr2 ']' ')'               {  $$ = createNode("ParseArgs", NULL, createNode("Id", $3, $5, 's'), 'c'); }
    |   PARSEINT '(' error ')'                          { flagError = 1; $$ = createNode("Dummy", NULL, NULL, 0); }
    ;

Expr2:
        Assignment                                      {  $$ = $1; }
    |   Expr                                            {  $$ = $1; }
    ;

Expr:
        '(' error ')'                                   { flagError = 1; $$ = createNode("Dummy", NULL, NULL, 0); }

    |   MethodInvocation                                {  $$ = $1; }
    |   ParseArgs                                       {  $$ = $1; }

    |   Expr AND Expr                                   {  $1->next = $3; $$ = createNode("And", NULL, $1, 'c'); }
    |   Expr OR Expr                                    {  $1->next = $3; $$ = createNode("Or", NULL, $1, 'c'); }

    |   Expr EQ Expr                                    {  $1->next = $3; $$ = createNode("Eq", NULL, $1, 'c'); }
    |   Expr GEQ Expr                                   {  $1->next = $3; $$ = createNode("Geq", NULL, $1, 'c'); }
    |   Expr '>' Expr                                   {  $1->next = $3; $$ = createNode("Gt", NULL, $1, 'c'); }
    |   Expr LEQ Expr                                   {  $1->next = $3; $$ = createNode("Leq", NULL, $1, 'c'); }
    |   Expr '<' Expr                                   {  $1->next = $3; $$ = createNode("Lt", NULL, $1, 'c'); }
    |   Expr NEQ Expr                                   {  $1->next = $3; $$ = createNode("Neq", NULL, $1, 'c'); }

    |   Expr '+' Expr                                   {  $1->next = $3; $$ = createNode("Add", NULL, $1, 'c'); }
    |   Expr '-' Expr                                   {  $1->next = $3; $$ = createNode("Sub", NULL, $1, 'c'); }
    |   Expr '*' Expr                                   {  $1->next = $3; $$ = createNode("Mul", NULL, $1, 'c'); }
    |   Expr '/' Expr                                   {  $1->next = $3; $$ = createNode("Div", NULL, $1, 'c'); }
    |   Expr '%' Expr                                   {  $1->next = $3; $$ = createNode("Mod", NULL, $1, 'c'); }

    |   '+' Expr        %prec UPLUS                     {  $$ = createNode("Plus", NULL, $2, 'c'); }
    |   '-' Expr        %prec UMINUS                    {  $$ = createNode("Minus", NULL, $2, 'c'); }
    |   '!' Expr                                        {  $$ = createNode("Not", NULL, $2, 'c'); }

    |   ID                                              {  $$ = createNode("Id", $1, NULL, 0); }
    |   ID DOTLENGTH                                    {  $$ = createNode("Length", NULL, createNode("Id", $1, NULL, 0), 'c'); }

    |   '(' Expr2 ')'                                    {  $$ = $2; }

    |   BOOLLIT                                         {  $$ = createNode("BoolLit", $1, NULL, 0); }
    |   DECLIT                                          {  $$ = createNode("DecLit", $1, NULL, 0); }
    |   REALLIT                                         {  $$ = createNode("RealLit", $1, NULL, 0); }
    ;

%%

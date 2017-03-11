%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(const char *s);
%}

%token BOOL, BOOLIT, CLASS, DO, DOTLENGTH, DOUBLE, ELSE, IF, INT, PARSEINT, PRINT, PUBLIC, RETURN, STATIC, STRING, VOID, WHILE
%token AND, OR, EQ, NEQ, LEQ, GEQ
%token RESERVED
%token ID, DECLIT, REALLIT, STRLIT

%%

/*calc: %empty                                    {;}
    | calc expression '\n'                      {printf("%d\n", $2);}

expression: expression '+' expression           {$$=$1+$3;}
        |   expression '-' expression           {$$=$1-$3;}
        |   expression '/' expression           {$$=$1/$3;}
        |   expression '*' expression           {$$=$1*$3;}
        |   NUMBER                              {$$=$1;}
        |   '(' expression ')'                  {$$=$2;}
        |   '-' expression        %prec UMINUS  {$$=-$2;}
        |   expression '%' expression           {$$=$1%$3;}
        ;
*/

%%

/* A função main() está do lado do lex */

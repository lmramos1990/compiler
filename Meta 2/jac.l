number  	[0-9]
letter   	[a-zA-Z]
text   		[a-zA-Z0-9]
%{
	#include <string.h>

	int flagM1 = 0, flagM2 = 1, col = 1, line = 1, colAux, lineAux;
%}

%X COMMENT_MULTI STRING

%%

"boolean" 												{ col += yyleng; if (flagM1) { printf("BOOL\n");} 					if (flagM2) { return BOOL;} }
"true"|"false"											{ col += yyleng; if (flagM1) { printf("BOOLLIT(%s)\n", yytext);} 	if (flagM2) { return BOOLLIT;} }
"class"													{ col += yyleng; if (flagM1) { printf("CLASS\n");} 					if (flagM2) { return CLASS;} }
"do"													{ col += yyleng; if (flagM1) { printf("DO\n");} 					if (flagM2) { return DO;} }
".length"												{ col += yyleng; if (flagM1) { printf("DOTLENGTH\n");} 				if (flagM2) { return DOTLENGTH;} }
"double"												{ col += yyleng; if (flagM1) { printf("DOUBLE\n");} 				if (flagM2) { return DOUBLE;} }
"else"													{ col += yyleng; if (flagM1) { printf("ELSE\n");} 					if (flagM2) { return ELSE;} }
"if"													{ col += yyleng; if (flagM1) { printf("IF\n");}						if (flagM2) { return IF;} }
"int"													{ col += yyleng; if (flagM1) { printf("INT\n");} 					if (flagM2) { return INT;} }
"Integer.parseInt"										{ col += yyleng; if (flagM1) { printf("PARSEINT\n");} 				if (flagM2) { return PARSEINT;} }
"System.out.println"									{ col += yyleng; if (flagM1) { printf("PRINT\n");} 					if (flagM2) { return PRINT;} }
"public"												{ col += yyleng; if (flagM1) { printf("PUBLIC\n");} 				if (flagM2) { return PUBLIC;} }
"return"												{ col += yyleng; if (flagM1) { printf("RETURN\n");} 				if (flagM2) { return RETURN;} }
"static"												{ col += yyleng; if (flagM1) { printf("STATIC\n");} 				if (flagM2) { return STATIC;} }
"String"												{ col += yyleng; if (flagM1) { printf("STRING\n");} 				if (flagM2) { return STRING;} }
"void"													{ col += yyleng; if (flagM1) { printf("VOID\n");} 					if (flagM2) { return VOID;} }
"while"													{ col += yyleng; if (flagM1) { printf("WHILE\n");} 					if (flagM2) { return WHILE;} }
"("														{ col += yyleng; if (flagM1) { printf("OCURV\n");} 					if (flagM2) { return '(';} }
")"														{ col += yyleng; if (flagM1) { printf("CCURV\n");} 					if (flagM2) { return ')';} }
"{"														{ col += yyleng; if (flagM1) { printf("OBRACE\n");}					if (flagM2) { return '{';} }
"}"														{ col += yyleng; if (flagM1) { printf("CBRACE\n");} 				if (flagM2) { return '}';} }
"["														{ col += yyleng; if (flagM1) { printf("OSQUARE\n");}				if (flagM2) { return '[';} }
"]"														{ col += yyleng; if (flagM1) { printf("CSQUARE\n");}				if (flagM2) { return ']';} }
"&&"													{ col += yyleng; if (flagM1) { printf("AND\n");} 					if (flagM2) { return AND;} }
"||"													{ col += yyleng; if (flagM1) { printf("OR\n");} 					if (flagM2) { return OR;} }
"<"														{ col += yyleng; if (flagM1) { printf("LT\n");} 					if (flagM2) { return '<';} }
">"														{ col += yyleng; if (flagM1) { printf("GT\n");} 					if (flagM2) { return '>';} }
"=="													{ col += yyleng; if (flagM1) { printf("EQ\n");} 					if (flagM2) { return EQ;} }
"!="													{ col += yyleng; if (flagM1) { printf("NEQ\n");} 					if (flagM2) { return NEQ;} }
"<="													{ col += yyleng; if (flagM1) { printf("LEQ\n");} 					if (flagM2) { return LEQ;} }
">="													{ col += yyleng; if (flagM1) { printf("GEQ\n");} 					if (flagM2) { return GEQ;} }
"+"														{ col += yyleng; if (flagM1) { printf("PLUS\n");} 					if (flagM2) { return '+';} }
"-"														{ col += yyleng; if (flagM1) { printf("MINUS\n");} 					if (flagM2) { return '-';} }
"*"														{ col += yyleng; if (flagM1) { printf("STAR\n");} 					if (flagM2) { return '*';} }
"/"														{ col += yyleng; if (flagM1) { printf("DIV\n");} 					if (flagM2) { return '/';} }
"%"														{ col += yyleng; if (flagM1) { printf("MOD\n");} 					if (flagM2) { return '%';} }
"!"														{ col += yyleng; if (flagM1) { printf("NOT\n");} 					if (flagM2) { return '!';} }
"="														{ col += yyleng; if (flagM1) { printf("ASSIGN\n");} 				if (flagM2) { return '=';} }
";"														{ col += yyleng; if (flagM1) { printf("SEMI\n");} 					if (flagM2) { return ';';} }
","														{ col += yyleng; if (flagM1) { printf("COMMA\n");} 					if (flagM2) { return ',';} }

"Integer"|"System"|"++"|"--"|"null"|"abstract"|"continue"|"for"|"new"|"switch"|"assert"|"default"|"goto"|"package"|"synchronized"|"boolean"|"do"|"if"|"private"|"this"|"break"|"double"|"implements"|"protected"|"throw"|"byte"|"else"|"import"|"public"|"throws"|"case"|"enum"|"instanceof"|"return"|"transient"|"catch"|"extends"|"int"|"short"|"try"|"char"|"final"|"interface"|"static"|"void"|"class"|"finally"|"long"|"strictfp"|"volatile"|"const"|"float"|"native"|"super"|"while"													{ col += yyleng;	if (flagM1) { printf("RESERVED(%s)\n", yytext);}	if (flagM2) { return RESERVED;} }

\/\/[^\n\r]*											{;}

"/*"													{ lineAux = line; 	colAux = col; 	col += yyleng; 	BEGIN COMMENT_MULTI; }
<COMMENT_MULTI>"*/"										{ col += yyleng; 	BEGIN 0; }
<COMMENT_MULTI>\r|\n|\r\n								{ line++; 			col = 1; }
<COMMENT_MULTI>"*"										{ col++; }
<COMMENT_MULTI>[^*\r\n]*								{ col += yyleng; }
<COMMENT_MULTI><<EOF>>									{ printf("Line %d, col %d: unterminated comment\n", lineAux, colAux); yyterminate(); }

({letter}|[_$])({text}|[_$])*							{ col += yyleng;	if (flagM1) { printf("ID(%s)\n", yytext);}		if (flagM2) { return ID;} }

{number}|[1-9]({number}|_)*{number}						{ col += yyleng;	if (flagM1) { printf("DECLIT(%s)\n", yytext);}	if (flagM2) { return DECLIT;} }

([0-9]([0-9]|_)*[0-9]|[0-9])(\.(([0-9]([0-9]|_)*[0-9]|[0-9])?([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?)?|([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9])))|\.([0-9]([0-9]|_)*[0-9]|[0-9])([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?		{ col += yyleng; if (flagM1) { printf("REALLIT(%s)\n", yytext);}	if (flagM2) { return REALLIT;} }

\"([^"\n\r\\]|(\\[fnrt\"\\]))*\"						{ col += yyleng;	if (flagM1) { printf("STRLIT(%s)\n", yytext);}	if (flagM2) { return STRLIT;} }
\"														{ colAux = col++; 	BEGIN STRING; }
<STRING>\"												{ col++; 			BEGIN 0;}
<STRING>([^"\n\r\\]|\\[fnrt\"\\])*						{ col += yyleng; }
<STRING>\\(.?)											{ printf("Line %d, col %d: invalid escape sequence (%s)\n", line, col, yytext);	col += yyleng; }
<STRING>\r\n|\n|\r										{ printf("Line %d, col %d: unterminated string literal\n", line, colAux); 		line++;		col = 1;	BEGIN 0; }
<STRING><<EOF>>											{ printf("Line %d, col %d: unterminated string literal\n", line, colAux); 		yyterminate(); }

" "|\t|\f												{ col++; }
\r\n|\n|\r												{ col = 1;	line++; }
.														{ printf("Line %d, col %d: illegal character (%s)\n", line, col, yytext); col++; }

%%

/*
{numero}(.{numero})*e[-+]{numero}																		{ printf("REALIT\n"); }
({numero}|[1-9]({numero}|_)*{numero})"."({numero}|[1-9]({numero}|_)*{numero})
({numero}|{numero}({numero}|_)*{numero})"." | "."({numero}|{numero}({numero}|_)*{numero})[eE][+-]?
([0-9]([0-9]|_)*[0-9]|[0-9])(\.(([0-9]([0-9]|_)*[0-9]|[0-9])?([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?)?|([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9])))|\.([0-9]([0-9]|_)*[0-9]|[0-9])([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?

*/


int main(int argc, char** argv) {
	if (argc > 1) {
		/*if (strcmp(argv[1], "-1") == 0)
			flagM1 = 0;*/
		if (strcmp(argv[1], "-l") == 0)
			flagM1 = 1;

		yylex();			/* call Lex entry point */
	} else {
		flagM2 = 1;
		yyparse();
	}

	col = line = 1;

	return 0;
}

int yywrap() {
	return 1;			/* Esta funcao sera chamada quando o input chegar ao fim. Devolve 1 se nao houver mais nada a processar */
}

void yyerror (char *s) {
	printf ("Line %d, col %d: %s: %s\n", line, col, s, yytext);
}

number  	[0-9]
letter   	[a-zA-Z]
text   		[a-zA-Z0-9]
%{
	#include <string.h>

	int flag = 0, col = 1, line = 1, colAux, lineAux;
%}

%X COMMENT_MULTI STRING

%%

"boolean" 																		{ col += yyleng; if (flag) { printf("BOOL\n");} }
"true"|"false"																	{ col += yyleng; if (flag) { printf("BOOLLIT(%s)\n", yytext);} }
"class"																			{ col += yyleng; if (flag) { printf("CLASS\n");} }
"do"																			{ col += yyleng; if (flag) { printf("DO\n");} }
".length"																		{ col += yyleng; if (flag) { printf("DOTLENGTH\n");} }
"double"																		{ col += yyleng; if (flag) { printf("DOUBLE\n");} }
"else"																			{ col += yyleng; if (flag) { printf("ELSE\n");} }
"if"																			{ col += yyleng; if (flag) { printf("IF\n");} }
"int"																			{ col += yyleng; if (flag) { printf("INT\n");} }
"Integer.parseInt"																{ col += yyleng; if (flag) { printf("PARSEINT\n");} }
"System.out.println"															{ col += yyleng; if (flag) { printf("PRINT\n");} }
"public"																		{ col += yyleng; if (flag) { printf("PUBLIC\n");} }
"return"																		{ col += yyleng; if (flag) { printf("RETURN\n");} }
"static"																		{ col += yyleng; if (flag) { printf("STATIC\n");} }
"String"																		{ col += yyleng; if (flag) { printf("STRING\n");} }
"void"																			{ col += yyleng; if (flag) { printf("VOID\n");} }
"while"																			{ col += yyleng; if (flag) { printf("WHILE\n");} }
"("																				{ col += yyleng; if (flag) { printf("OCURV\n");} }
")"																				{ col += yyleng; if (flag) { printf("CCURV\n");} }
"{"																				{ col += yyleng; if (flag) { printf("OBRACE\n");} }
"}"																				{ col += yyleng; if (flag) { printf("CBRACE\n");} }
"["																				{ col += yyleng; if (flag) { printf("OSQUARE\n");} }
"]"																				{ col += yyleng; if (flag) { printf("CSQUARE\n");} }
"&&"																			{ col += yyleng; if (flag) { printf("AND\n");} }
"||"																			{ col += yyleng; if (flag) { printf("OR\n");} }
"<"																				{ col += yyleng; if (flag) { printf("LT\n");} }
">"																				{ col += yyleng; if (flag) { printf("GT\n");} }
"=="																			{ col += yyleng; if (flag) { printf("EQ\n");} }
"!="																			{ col += yyleng; if (flag) { printf("NEQ\n");} }
"<="																			{ col += yyleng; if (flag) { printf("LEQ\n");} }
">="																			{ col += yyleng; if (flag) { printf("GEQ\n");} }
"+"																				{ col += yyleng; if (flag) { printf("PLUS\n");} }
"-"																				{ col += yyleng; if (flag) { printf("MINUS\n");} }
"*"																				{ col += yyleng; if (flag) { printf("STAR\n");} }
"/"																				{ col += yyleng; if (flag) { printf("DIV\n");} }
"%"																				{ col += yyleng; if (flag) { printf("MOD\n");} }
"!"																				{ col += yyleng; if (flag) { printf("NOT\n");} }
"="																				{ col += yyleng; if (flag) { printf("ASSIGN\n");} }
";"																				{ col += yyleng; if (flag) { printf("SEMI\n");} }
","																				{ col += yyleng; if (flag) { printf("COMMA\n");} }

"Integer"|"System"|"++"|"--"|"null"|"abstract"|"continue"|"for"|"new"|"switch"|"assert"|"default"|"goto"|"package"|"synchronized"|"boolean"|"do"|"if"|"private"|"this"|"break"|"double"|"implements"|"protected"|"throw"|"byte"|"else"|"import"|"public"|"throws"|"case"|"enum"|"instanceof"|"return"|"transient"|"catch"|"extends"|"int"|"short"|"try"|"char"|"final"|"interface"|"static"|"void"|"class"|"finally"|"long"|"strictfp"|"volatile"|"const"|"float"|"native"|"super"|"while"													{ col += yyleng; if (flag) { printf("RESERVED(%s)\n", yytext);} }

\/\/[^\n\r]*																	{;}

"/*"																			{ lineAux = line; colAux = col; col += yyleng; BEGIN COMMENT_MULTI; }
<COMMENT_MULTI>"*/"																{ col += yyleng; BEGIN 0; }
<COMMENT_MULTI>\r|\n|\r\n														{ line++; col = 1; }
<COMMENT_MULTI>"*"																{ col++; }
<COMMENT_MULTI>[^*\r\n]*														{ col += yyleng; }
<COMMENT_MULTI><<EOF>>															{ printf("Line %d, col %d: unterminated comment\n", lineAux, colAux); yyterminate(); }

({letter}|[_$])({text}|[_$])*													{ col += yyleng; if (flag) { printf("ID(%s)\n", yytext);} }

{number}|[1-9]({number}|_)*{number}												{ col += yyleng; if (flag) { printf("DECLIT(%s)\n", yytext);} }

([0-9]([0-9]|_)*[0-9]|[0-9])(\.(([0-9]([0-9]|_)*[0-9]|[0-9])?([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?)?|([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9])))|\.([0-9]([0-9]|_)*[0-9]|[0-9])([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?		{ col += yyleng; if (flag) { printf("REALLIT(%s)\n", yytext);} }

\"([^"\n\r\\]|(\\[fnrt\"\\]))*\"												{ col += yyleng; if (flag) { printf("STRLIT(%s)\n", yytext);} }
\"																				{ colAux = col++; BEGIN STRING; }
<STRING>\"																		{ col++; BEGIN 0;}
<STRING>([^"\n\r\\]|\\[fnrt\"\\])*												{ col += yyleng; }
<STRING>\\(.?)																	{ printf("Line %d, col %d: invalid escape sequence (%s)\n", line, col, yytext); col += yyleng; }
<STRING>\r\n|\n|\r																{ printf("Line %d, col %d: unterminated string literal\n", line, colAux); line++; col = 1; BEGIN 0; }
<STRING><<EOF>>																	{ printf("Line %d, col %d: unterminated string literal\n", line, colAux); yyterminate(); }

" "|\t|\f																		{ col++; }
\r\n|\n|\r																		{ col = 1; line++; }
.																				{ printf("Line %d, col %d: illegal character (%s)\n", line, col, yytext); col++; }

%%

/*
{numero}(.{numero})*e[-+]{numero}																		{ printf("REALIT\n"); }
({numero}|[1-9]({numero}|_)*{numero})"."({numero}|[1-9]({numero}|_)*{numero})
({numero}|{numero}({numero}|_)*{numero})"." | "."({numero}|{numero}({numero}|_)*{numero})[eE][+-]?
([0-9]([0-9]|_)*[0-9]|[0-9])(\.(([0-9]([0-9]|_)*[0-9]|[0-9])([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?)?|([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9])))|\.([0-9]([0-9]|_)*[0-9]|[0-9])([eE][+-]?([0-9]([0-9]|_)*[0-9]|[0-9]))?

*/


int main(int argc, char** argv) {
	if (argc > 1 && (strcmp(argv[1], "-1") == 0 || strcmp(argv[1], "-l") == 0)) {
		flag = 1;
	}

	yylex();			/* chama o entry point */
	return 0;
}

int yywrap() {
	return 1;			/* Esta funcao sera chamada quando o input chegar ao fim. Devolve 1 se nao houver mais nada a processar */
}

rm compiler y.tab.c
lex jac.l
yacc -d -v jac.y
clang-3.8 -o compiler -Wall -Wno-unused-function tree.c y.tab.c lex.yy.c
./compiler < Echo.ja
zip jac.zip jac.l jac.y structs.h tree.c

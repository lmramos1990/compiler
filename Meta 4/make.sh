rm compiler y.tab.c
lex jac.l
yacc -d -v jac.y
clang-3.8 -o compiler -g -Wall -Wno-unused-function ast.c table.c llvm.c y.tab.c lex.yy.c
./compiler -s < Echo.ja
./compiler < Echo.ja > Echo.ll
lli-3.8 Echo.ll 5 1
zip jac.zip jac.l jac.y structs.h ast.c table.c llvm.c

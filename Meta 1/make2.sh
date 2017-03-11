rm compiler
lex jac.l
clang-3.8 -o compiler lex.yy.c
./compiler -l < Echo2.ja
zip jac.zip jac.l

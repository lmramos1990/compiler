rm compiler y.tab.c
lex jac.l
yacc -d -v jac.y
clang-3.8 -o compiler -g -Wall -Wno-unused-function ast.c table.c y.tab.c lex.yy.c
./compiler -s < Echo.ja > Echo.out
./compiler -s < Echo2.ja > Echo2.out
./compiler -s < gcd.ja > gcd.out
./compiler -s < gcd2.ja > gcd2.out
./compiler -s < gcd3.ja > gcd3.out
./compiler -s < gcd4.ja > gcd4.out
./compiler -s < gcd5.ja > gcd5.out
./compiler -s < gcd6.ja > gcd6.out
zip jac.zip jac.l jac.y structs.h ast.c table.c

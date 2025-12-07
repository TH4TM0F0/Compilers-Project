all:
	bison -d -v parser.y
	flex lexer.l
	gcc lex.yy.c parser.tab.c -o compiler.exe
	./compiler.exe

check:
	bison -d -v parser.y
	flex lexer.l

clean:

	rm -f lex.yy.c parser.tab.h parser.tab.c *.exe

runTest:
	gcc test.c
	./a.exe

lexerTest:
	make clean
	clear
	flex lexer.l
	gcc lex.yy.c
	./a.exe input.txt
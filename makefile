CC      = gcc
CFLAGS  = -Wall -IInclude
LDFLAGS = -lm

OBJS = parser.tab.c lex.yy.c \
       Source/Assembler.c \
       Source/ErrorHandler.c \
       Source/Parameter.c \
       Source/Quadruple.c

OUT = exe/compiler.exe

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h parser.output compiler.exe
	rm -rf exe

build:
	mkdir -p exe
	bison -d parser.y
	flex lexer.l
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT) $(LDFLAGS)

run:
	./$(OUT) "Text Files/input.txt" || true

all: clean build run



symtabletest:
	gcc -IInclude Source/SymbolTable.c -o test
	.\test.exe

testt: 
	gcc test.c -o test.output
	./test.output
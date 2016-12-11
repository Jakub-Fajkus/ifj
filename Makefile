SRC=debug.c doublelinkedlist.c expressionanalizer.c ifj16.c instruction.c interpret.c lexicalanalyzer.c main.c semanticalanalyzer.c stack.c symboltable.c syntacticalanalyzer.c
CFLAGS=-std=c99 -DNOT_DEBUG

all: $(SRC)
	gcc -o ifj16 $^ $(CFLAGS)


testScript:
	bash ./test

test: testScript
tests: testScript


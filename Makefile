SRC=$(wildcard *.c)
CFLAGS=-std=c99 -DNOT_DEBUG

all: $(SRC)
	gcc -o ifj16 $^ $(CFLAGS)


testScript:
	bash ./test

test: testScript
tests: testScript


SRC=$(wildcard *.c)
CFLAGS=-std=c99

all: $(SRC)
	gcc -o ifj16 $^ $(CFLAGS) -DNOT_DEBUG


testScript:
	bash ./test

test: testScript
tests: testScript


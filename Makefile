SRC=$(wildcard *.c)
CFLAGS=-std=c99

testScript:
	bash ./test

all: $(SRC)
	gcc -o ifj16 $^ $(CFLAGS) -DNOT_DEBUG

test: testScript
tests: testScript


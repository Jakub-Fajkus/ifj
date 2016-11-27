SRC=$(wildcard *.c)
CFLAGS=-std=c99

all: $(SRC)
	gcc -o IFJ16 $^ $(CFLAGS) -DNOT_DEBUG
	gcc -o ifj16 $^ $(CFLAGS) -DNOT_DEBUG

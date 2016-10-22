
PRJ=ifj16
#
PROGS=$(PRJ)-test
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ)-test: $(PRJ).c $(PRJ)-test.c ../IAL/ial.c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c $(PRJ)-test.c ../IAL/ial.c

clean:
	rm -f *.o *.out $(PROGS)
#
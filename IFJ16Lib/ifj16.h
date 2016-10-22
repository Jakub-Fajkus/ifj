#ifndef	_IFJ16_H_
#define	_IFJ16_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<stdbool.h>

int ifj16_readInt();

double ifj16_readDouble();

char *ifj16_readString();

void ifj16_print(char *s);

int ifj16_length(char *);

char *ifj16_substr(char *, int, int);

int ifj16_compare(char *, char *);

int ifj16_find(char *, char *);

char *ifj16_sort(char *s);

#endif
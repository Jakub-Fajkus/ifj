//
// Created by rockuo on 9.10.16.
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdarg.h>

int readInt();

double readDouble();

char *readString();

void print( /*some multi argument magic using va_list*/ );

int length(char *);

char *substr(char *, int, int);

int compare(char *, char *);

int find(char *, char *);

char *sort(char *);
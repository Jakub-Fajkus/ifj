#ifndef	_IAL_H_
#define	_IAL_H_

#include <stdbool.h>

//************** SYMBOL TABLE **************

void make_delta(int *delta, unsigned char *pat, int patlen);

int boyer_moore (unsigned char *string, unsigned int stringlen, unsigned char *pat, unsigned int patlen);

char* quickSortWrapper(char *s);

void quickSort(char *arr, int left, int right);

char medianIndex(char *s);

void swap(char *arr, int left, int right);

#endif
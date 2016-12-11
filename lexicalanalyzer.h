/**
 * Implementace interpretu imperativního jazyka IFJ16.
 *
 * Richard Bureš
 */

#ifndef IFJ_LEXICALANALYZER_H
#define IFJ_LEXICALANALYZER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<stdbool.h>

#include "lexicalanalyzerstructures.h"

void initializeStream(char *);

void destroyStream();

TOKEN *getToken();

#endif

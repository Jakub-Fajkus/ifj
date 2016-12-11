/**
 * Implementace interpretu imperativního jazyka IFJ16.
 *
 * Jakub Fajkus
 * Richard Bureš
 */

#ifndef IFJ_LEXICALANALYZERSTRUCTURES_H
#define IFJ_LEXICALANALYZERSTRUCTURES_H

#include <stdbool.h>

typedef enum {
    KEYWORD = 2000,
    IDENTIFIER_FULL = 2001,
    IDENTIFIER = 2002,
    LITERAL_STRING = 2003,
    LITERAL_INTEGER = 2004,
    LITERAL_DOUBLE = 2005,
    OPERATOR_ARITHMETIC = 2006,
    OPERATOR_LOGIC = 2007,
    OPERATOR_ASSIGN = 2008,
    SEPARATOR = 2009,
    BRACKET = 2010,
    SEMICOLON = 2011,
    END_OF_FILE = 2012,
    LEX_ERROR = 2013
} TOKEN_TYPE;

typedef struct {
    char name;
} BRACKET_INFO;

typedef struct {
    char *name;
} KEYWORD_INFO;

typedef struct {
    char *name;
} SHORT_ID_INFO;

typedef struct {
    char *name;
} OPERATOR_LOGIC_INFO;

typedef struct {
    int value;
} LITERAL_INTEGER_INFO;

typedef struct {
    double value;
} LITERAL_DOUBLE_INFO;

typedef struct {
    char * name ;
} LITERAL_STRING_INFO;

typedef struct {
    char name;
} OPERATOR_ARITHMETIC_INFO;

typedef struct {
    char *class;
    char *name;
} IDENTIFIER_FULL_INFO;

typedef struct {
    char *name;
} IDENTIFIER_INFO;

typedef union {
    SHORT_ID_INFO shortID;
    BRACKET_INFO bracket;
    OPERATOR_LOGIC_INFO operatorLogic;
    OPERATOR_ARITHMETIC_INFO operatorArithmetic;
    LITERAL_DOUBLE_INFO numberDouble;
    LITERAL_INTEGER_INFO numberInteger;
    IDENTIFIER_INFO identifier;
    IDENTIFIER_FULL_INFO identifierFull;
    LITERAL_STRING_INFO literalString;
    KEYWORD_INFO keyword;
} DATA;

typedef struct {
    TOKEN_TYPE type;
    DATA data;
} TOKEN;

#endif //IFJ_LEXICALANALYZERSTRUCTURES_H

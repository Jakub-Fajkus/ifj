//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_LEXICALANALYZERSTRUCTURES_H
#define IFJ_LEXICALANALYZERSTRUCTURES_H

#include <stdbool.h>

typedef enum {
    KEYWORD,
    IDENTIFIER_FULL,
    IDENTIFIER,
    LITERAL_BOOL,
    LITERAL_STRING,
    LITERAL_INTEGER,
    LITERAL_DOUBLE,
    OPERATOR_ARITHMETIC,
    OPERATOR_LOGIC,
    OPERATOR_ASSIGN,
    SEPARATOR,
    BRACKET,
    SEMICOLON,
    END_OF_FILE,
    LEX_ERROR
} TOKEN_TYPE;

typedef struct {
    char name;
} BRACKET_INFO;

typedef struct {
    char *name;
} KEYWORD_INFO;

typedef struct {
    bool value;
} LITERAL_BOOL_INFO;

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
    LITERAL_BOOL_INFO literalBool;
    KEYWORD_INFO keyword;
} DATA;

typedef struct {
    TOKEN_TYPE type;
    DATA data;
} TOKEN;

#endif //IFJ_LEXICALANALYZERSTRUCTURES_H

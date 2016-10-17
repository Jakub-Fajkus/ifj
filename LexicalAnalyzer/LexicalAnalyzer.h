//
// Created by rockuo on 9.10.16.
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<stdbool.h>

typedef enum {
    KEYWORD,
    IDENTIFIER_FULL,
    IDENTIFIER,
    LITERAL_INTEGER,
    LITERAL_DOUBLE,
    LITERAL_BOOL,
    LITERAL_STRING,
    NUMBER_INTEGER,
    NUMBER_DOUBLE,
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
} SHORT_ID_INFO;

typedef struct {
    char *name;
} OPERATOR_LOGIC_INFO;

typedef struct {
    int value;
} NUMBER_INTEGER_INFO;

typedef struct {
    double value;
} NUMBER_DOUBLE_INFO;

typedef struct {
    char name;
} OPERATOR_ARITHMETIC_INFO;

typedef union {
    SHORT_ID_INFO shortID;
    BRACKET_INFO bracket;
    OPERATOR_LOGIC_INFO operatorLogic;
    OPERATOR_ARITHMETIC_INFO operatorArithmetic;
    NUMBER_DOUBLE_INFO numberDouble;
    NUMBER_INTEGER_INFO numberInteger;
} DATA;

typedef struct {
    TOKEN_TYPE type;
    DATA data;
} TOKEN;

void initializeStream(char *);

void destroyStream();

TOKEN *getToken();



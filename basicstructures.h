//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_BASICSTRUCTURES_H
#define IFJ_BASICSTRUCTURES_H

#include <stdbool.h>

typedef enum{
    TYPE_INT = 1000,
    TYPE_DOUBLE = 1001,
    TYPE_STRING = 1002,
    TYPE_VOID = 1003,
    TYPE_BOOL = 1004
} DATA_TYPE;

typedef struct {
    char* name;
    DATA_TYPE type;
} FUNCTION_PARAMETER;

typedef enum{
    TREE_NODE_VARIABLE = 1010,
    TREE_NODE_FUNCTION = 1011
} TREE_NODE_DATA_TYPE;

typedef union {
    int intValue;
    double doubleValue;
    char *stringValue;
} VARIABLE_VALUE;

typedef struct {
    char *name;
    DATA_TYPE type;
    VARIABLE_VALUE value;
    bool initialized;
} VARIABLE;

#endif //IFJ_BASICSTRUCTURES_H

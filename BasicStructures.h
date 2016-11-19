//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_BASICSTRUCTURES_H
#define IFJ_BASICSTRUCTURES_H

typedef enum{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_VOID
} DATA_TYPE;

typedef struct {
    char* name;
    DATA_TYPE type;
} FUNCTION_PARAMETER;

typedef enum{
    TREE_NODE_VARIABLE,
    TREE_NODE_FUNCTION,
    TREE_NODE_FUNCTION_CALL,
} TREE_NODE_DATA_TYPE;

typedef union {
    int intValue;
    double doubleValue;
    char *stringValue;
} VARIABLE_VALUE;

typedef struct {
    DATA_TYPE var_type;
    VARIABLE_VALUE var_value;
} VARIABLE;

#endif //IFJ_BASICSTRUCTURES_H

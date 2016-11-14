//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_BASICSTRUCTURES_H
#define IFJ_BASICSTRUCTURES_H

typedef enum{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING
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

#endif //IFJ_BASICSTRUCTURES_H

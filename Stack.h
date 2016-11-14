//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_STACK_H
#define IFJ_STACK_H

#include "SymbolTable.h"

#define STACK_SIZE_LIMIT 100000

typedef enum STACK_ELEMENT_TYPE {
    STACK_ELEMENT_FUNCTION_CALL,
    //todo: use for expressions(exp, operator, <, > ...)
}STACK_ELEMENT_TYPE;

typedef union STACK_ELEMENT_DATA {
    SYMBOL_TABLE_FUNCTION_CALL functionCall;
    //todo: use for expressions

}STACK_ELEMENT_DATA;

typedef struct STACK_ELEMENT{
    enum STACK_ELEMENT_TYPE type;
    union STACK_ELEMENT_DATA data;
}STACK_ELEMENT;

typedef struct {
    struct STACK_ELEMENT* arr;              /* pole pro uložení hodnot */
    int top;                                /* index prvku na vrcholu zásobníku */
    int maxSize;
    int actualSize;
} tStack;

void stackInit ( tStack* s );
int stackEmpty ( const tStack* s );
int stackFull ( const tStack* s );
void stackTop ( const tStack* s, struct STACK_ELEMENT* element );
void stackPop ( tStack* s );
void stackPush ( tStack* s, struct STACK_ELEMENT element );

#endif //IFJ_STACK_H

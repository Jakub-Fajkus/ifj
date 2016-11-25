//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_STACK_H
#define IFJ_STACK_H

#include "SymbolTable.h"
#include "ExpressionAnalizer.h"
#include <stdbool.h>

#define STACK_SIZE_LIMIT 100000

typedef enum STACK_ELEMENT_TYPE {
    STACK_ELEMENT_TYPE_EXPRESSION = 15000,
    EA_TERMINAL = 15001,
    EA_NOT_TERMINAL = 15002,
    EA_TERMINAL_ACTION = 15003,
    STACK_ELEMENT_TYPE_LOCAL_FRAME = 15004,
    STACK_ELEMENT_TYPE_SYMBOL_TABLE_PTR = 15005
}STACK_ELEMENT_TYPE;

typedef enum EA_TERMINAL_TYPE_ENUM{
    EA_ADD = 0,
    EA_SUB = 1,
    EA_MUL = 2,
    EA_LEFT_BR = 3,
    EA_RIGHT_BR = 4,
    EA_I = 5,
    EA_DIV = 6,
    EA_IS_LESS = 7,
    EA_IS_MORE = 8,
    EA_IS_LESS_EQUAL = 9,
    EA_IS_MORE_EQUAL = 10,
    EA_IS_EQUAL = 11,
    EA_IS_NOT_EQUAL = 12,
    EA_START_END = 13,
    EA_EMPTY,
    EA_UNKNOWN
} EA_TERMINAL_TYPE;

typedef struct {
    EA_TERMINAL_TYPE type;
    TOKEN token;
} EA_TERMINAL_DATA;

typedef struct {
    char *name;//bude bohate stačit
    DATA_TYPE type;
} EA_NOT_TERMINAL_DATA;

typedef union STACK_ELEMENT_DATA {
    EA_TERMINAL_DATA terminalData;
    tDLList *localFrame;
    EA_NOT_TERMINAL_DATA notTerminalData;
    EA_TERMINAL_TYPE actionType;
    struct SYMBOL_TABLE_NODE *symbolTableNode;
}STACK_ELEMENT_DATA;

typedef struct STACK_ELEMENT{
    enum STACK_ELEMENT_TYPE type;
    union STACK_ELEMENT_DATA data;

}STACK_ELEMENT;

typedef struct STACK_STR{
    struct STACK_ELEMENT *arr;              /* pole pro uložení hodnot */
    int top;                                /* index prvku na vrcholu zásobníku */
    int maxSize;
    int actualSize;
} tStack;

void stackInit ( struct STACK_STR* s );
int stackEmpty ( const struct STACK_STR* s );
int stackFull ( const struct STACK_STR* s );
void stackTop ( const struct STACK_STR* s, struct STACK_ELEMENT* element );
void stackPop ( struct STACK_STR* s );
void stackPush ( struct STACK_STR* s, struct STACK_ELEMENT element );

#endif //IFJ_STACK_H
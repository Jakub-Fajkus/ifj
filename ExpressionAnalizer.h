//
// Created by rockuo on 19.11.16.
//

#ifndef IFJ_EXPRESSIONANALIZER_H
#define IFJ_EXPRESSIONANALIZER_H

#include "DoubleLinkedList.h"
#include "SyntacticalAnalyzer.h"
#include <stdio.h>
#include <stdbool.h>

void parseExpression(tDLList *, tDLList *);

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

#include "Stack.h" //include it after EA_TERMINAL_TYPE_ENUM which is needed by Stack.h


#endif //IFJ_EXPRESSIONANALIZER_H

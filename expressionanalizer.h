//
// Created by rockuo on 19.11.16.
//

#ifndef IFJ_EXPRESSIONANALIZER_H
#define IFJ_EXPRESSIONANALIZER_H

#include "doublelinkedlist.h"
#include "syntacticalanalyzer.h"
#include <stdio.h>
#include "instruction.h"
#include <stdbool.h>
#include "basicstructures.h"
#include "symboltable.h"

#include "stack.h"

int parseExpression(tDLList *threeAddressCode, char **returnValName, DATA_TYPE *returnValType,bool firstPass);

#endif //IFJ_EXPRESSIONANALIZER_H

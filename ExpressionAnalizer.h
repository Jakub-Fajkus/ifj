//
// Created by rockuo on 19.11.16.
//

#ifndef IFJ_EXPRESSIONANALIZER_H
#define IFJ_EXPRESSIONANALIZER_H

#include "DoubleLinkedList.h"
#include "SyntacticalAnalyzer.h"
#include <stdio.h>
#include "instruction.h"
#include <stdbool.h>
#include "BasicStructures.h"
#include "SymbolTable.h"

#include "Stack.h" //include it after EA_TERMINAL_TYPE_ENUM which is needed by Stack.h

//todo: return true if the expression was successfully parsed, false otherwise... use exit only in error situations(99)
int parseExpression(tDLList *threeAddressCode, char **returnValName, DATA_TYPE *returnValType,bool firstPass);

#endif //IFJ_EXPRESSIONANALIZER_H

//
// Created by Jakub Fajkus on 17.11.16.
//

#ifndef IFJ_DEBUG_H
#define IFJ_DEBUG_H

#include "DoubleLinkedList.h"
#include "Stack.h"

void printAllTokens(tDLList *list);
void printToken(TOKEN *token);
void printEnumValue(int value);
void printToken(TOKEN *token);
void printStack(tStack stack);
void printEaTerminalType(enum EA_TERMINAL_TYPE_ENUM type);
#endif //IFJ_DEBUG_H

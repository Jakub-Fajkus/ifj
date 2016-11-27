//
// Created by Jakub Fajkus on 17.11.16.
//

#ifndef IFJ_DEBUG_H
#define IFJ_DEBUG_H

#include "doublelinkedlist.h"
#include "stack.h"

#ifndef NOT_DEBUG
    #define debugPrintf(...) printf(__VA_ARGS__)
#else
    #define debugPrintf(...)
#endif

void printAllTokens(tDLList *list);
void printToken(TOKEN *token);
void printEnumValue(int value);
void printToken(TOKEN *token);
void printStack(tStack stack);
void printEaTerminalType(enum EA_TERMINAL_TYPE_ENUM type);
void printInstruction(INSTRUCTION *instruction);
void printInstructions(tDLList *instructions);
#endif //IFJ_DEBUG_H

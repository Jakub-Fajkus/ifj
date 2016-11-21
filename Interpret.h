//
// Created by Andrej on 19.11.2016.
//

#ifndef IFJ_INTERPRET_H
#define IFJ_INTERPRET_H

#include "BasicStructures.h"
#include "DoubleLinkedList.h"
#include "instruction.h"


int Interpret(tDLList *InstructionList);
void exitInterpret(int errNumber);
void checkMalloc(void *ptr);
void InstructionExecute(INSTRUCTION *Instr);    // Function for global execution of instructions

#endif //IFJ_INTERPRET_H

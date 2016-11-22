//
// Created by Andrej on 19.11.2016.
//

#ifndef IFJ_INTERPRET_H
#define IFJ_INTERPRET_H

#include "BasicStructures.h"
#include "DoubleLinkedList.h"
#include "instruction.h"
#include <string.h>


int Interpret(tDLList *InstructionList);
void exitInterpret(int errNumber);
void checkMalloc(void *ptr);



/* ************************************************ EXECUTE ************************************************/

//TODO: remake the types of functions
void InstructionExecute(INSTRUCTION *Instr);
void executeInstructionMathOperation(INSTRUCTION *instr);

void executeInstructionAssign(VARIABLE *dst, VARIABLE *src); // done

#endif //IFJ_INTERPRET_H

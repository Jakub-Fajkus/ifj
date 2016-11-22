//
// Created by Andrej on 19.11.2016.
//

#ifndef IFJ_INTERPRET_H
#define IFJ_INTERPRET_H

#include "BasicStructures.h"
#include "DoubleLinkedList.h"
#include "instruction.h"
#include <string.h>

/**
 * @param InstructionList - list of instructons passed by parser
 * @param globalFrame - Important note: function interpret passes this argument to itself
 *                      TL;DR: Interpret called from parser passes NULL value
 * @param stackOfLocalFrames - Important note: function interpret passes this argument to itself
 *                      TL;DR: Interpret called from parser passes NULL value
 * @return int value in case of crash if the interpret, to make sure the function main can free() everything else
 */
int Interpret( tDLList *InstructionList, tDLList *globalFrame, tStack *stackOfLocalFrames );

/// TO BE REPLACED WITH ANOTHER SOLUTION
/// \param errNumber - exit number
void exitInterpret(int errNumber);

/// TO BE REPLACED WITH ANOTHER SOLUTION
/// \param ptr - any type of pointer if it is NULL
void checkMalloc(void *ptr);



/* ************************************************ EXECUTE ************************************************/

//TODO: remake the types of functions

/**
 * This functiont
 * @param Instr
 */
void InstructionExecute(INSTRUCTION *Instr);
void executeInstructionMathOperation(INSTRUCTION *instr);

void executeInstructionAssign(VARIABLE *dst, VARIABLE *src); // done

#endif //IFJ_INTERPRET_H

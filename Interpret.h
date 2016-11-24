//
// Created by Andrej on 19.11.2016.
//

#ifndef IFJ_INTERPRET_H
#define IFJ_INTERPRET_H

#include "BasicStructures.h"
#include "DoubleLinkedList.h"
#include "instruction.h"
#include "Stack.h"
#include <string.h>

/**
 * @param InstructionList - list of instructons passed by parser
 * @param globalFrame - Important note: function interpret passes this argument to itself
 *                      TL;DR: Interpret called from parser passes NULL value
 * @param stackOfLocalFrames - Important note: function interpret passes this argument to itself
 *                      TL;DR: Interpret called from parser passes NULL value
 * @return int value in case of crash if the interpret, to make sure the function main can free() everything else
 */
/*int Interpret( tDLList *InstructionList, tDLList *globalFrame, tStack *stackOfLocalFrames );*/

/// TO BE REPLACED WITH ANOTHER SOLUTION
/// \param errNumber - exit number
void exitInterpret(int errNumber);

/// TO BE REPLACED WITH ANOTHER SOLUTION
/// \param ptr - any type of pointer if it is NULL
void checkMalloc(void *ptr);



/* ************************************************ EXECUTE ************************************************/

//TODO: remake the types of functions

/**
 * GENERAL FUNCTION FOR EXECUTION OF ANY INSTRUCITON
 * EXCEPTIONS HAVE THEIR OWN FUNCTIONS
 * @param Instr
 */
void InstructionExecute(INSTRUCTION *Instr);

/**
 * ADD (math & concatenation), SUB, MUL, DIV
 * ANALOGICAL TO createInstructionMathOperation()
 * @param instr
 */
void executeInstructionMathOperation(INSTRUCTION *instr);

/**
 * ASSIGN execution
 * ANALOGICAL TO createInstructionAssign()
 * @param dst
 * @param src
 */
void executeInstructionAssign(VARIABLE *dst, VARIABLE *src); // done

/**
 * Evaluation of bool expression
 * Accepted Instruction types:
 *      Instruction_Bool_Equals:    if ( src1 == src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_EqualsNot, if ( src1 != src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_More,      if ( src1 > src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_Less,      if ( src1 < src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_MoreEqual, if ( src1 >= src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_LessEqual, if ( src1 <= src2 ) dst=TRUE else dst=FALSE;
 * @param instr: accepts type, dst, src1, src2
 */
void executeInstructionExpressionEvaluation(INSTRUCTION *instr);

void executeInstructionIf(INSTRUCTION *instr);


#endif //IFJ_INTERPRET_H

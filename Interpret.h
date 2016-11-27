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
int Interpret( tDLList *InstructionList, tDLList *globalFrame, tStack *stackOfLocalFrames );


/* ************************************************ EXECUTE ************************************************/

/**
* ASSIGN execution
* ANALOGICAL TO createInstructionAssign()
* @param dst
* @param src
* @return 0 or 6
*/
int executeInstructionAssign(VARIABLE *dst, VARIABLE *src);

/**
 * Executes these operations in format: DST = SRC1 %% SRC2, where %% is operator
 * ADD (math & concatenation), SUB, MUL, DIV
 * ANALOGICAL TO createInstructionMathOperation()
 *
 * @param instrType
 * @param dst - ptr to value
 * @param src1 - ptr to value
 * @param src2 - ptr to value
 * @return 99 - intern error, 10- other runtime error, 9- division by zero, 0- OK
 */
int executeInstructionMathOperation(INSTRUCTION_TYPE instrType, VARIABLE *dst, VARIABLE *src1, VARIABLE *src2);

/**
 * Evaluation of bool expression
 * Accepted Instruction types:
 *      Instruction_Bool_Equals:    if ( src1 == src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_EqualsNot, if ( src1 != src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_More,      if ( src1 > src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_Less,      if ( src1 < src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_MoreEqual, if ( src1 >= src2 ) dst=TRUE else dst=FALSE;
 *      Instruction_Bool_LessEqual, if ( src1 <= src2 ) dst=TRUE else dst=FALSE;
 * @param instrType
 * @param dst - Names of variables
 * @param src1
 * @param src
 */
int executeInstructionExpressionEvaluation(INSTRUCTION_TYPE instrType, VARIABLE *dst, VARIABLE *src1, VARIABLE *src2);



//----------------------------------REST

/**
 * GENERAL FUNCTION FOR EXECUTION OF ANY INSTRUCITON
 * EXCEPTIONS HAVE THEIR OWN FUNCTIONS
 * @param Instr
 */
void InstructionExecute(INSTRUCTION *Instr);

int callInterpret();
void exitInterpret(int errNumber);
void checkMalloc(void *ptr);

#endif //IFJ_INTERPRET_H

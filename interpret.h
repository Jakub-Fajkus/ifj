/**
 * Implementace interpretu imperativního jazyka IFJ16.
 *
 * Jakub Fajkus
 * Richard Bureš
 * Andrej Hučko
 */

#ifndef IFJ_INTERPRET_H
#define IFJ_INTERPRET_H

#include "basicstructures.h"
#include "doublelinkedlist.h"
#include "instruction.h"
#include "stack.h"
#include <string.h>


/**
 * ~ Interpret instruction executor ~
 *
 * @param InstructionList - Our beloved instruction list
 * @param globalFrame - Important note: function interpret passes this argument to itself
 *                      TL;DR: Interpret called from parser passes NULL value
 * @param stackOfLocalFrames - Important note: function interpret passes this argument to itself
 *                      TL;DR: Interpret called from parser passes NULL value
 * @param activeFunction - POINTER TO SYMBOL TABLE. I want to get the name of the function & it's return type.
 * @param checkReturn - important for runtime error 8!
 * @return
 */
int Interpret( tDLList *InstructionList, tDLList *globalFrame, tStack *stackOfLocalFrames, struct SYMBOL_TABLE_FUNCTION_STR *activeFunction, bool checkReturn );


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




/**
 * Execution of Built-in functions
 * @param instrType
 * @param dst
 * @param src1
 * @param src2
 * @return
 */
int executeInstructionBuiltInFunction(INSTRUCTION_TYPE instrType, VARIABLE *dst, VARIABLE *src1, VARIABLE *src2);


char* getClassNameWithDotFromFullIdentifier(char *fullIdentifier);

#endif //IFJ_INTERPRET_H
